#include "font/raster/outline_rasterizer.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "math/curve/bezier.hpp"
#include "math/utils.hpp"
#include "math/vec2.hpp"

namespace robot {

namespace {

// 曲線細分後折線與曲線的最大距離（像素）。比半個像素小，細分造成的誤差在反鋸齒後看不出來。
constexpr float FLATTEN_TOLERANCE = 0.25f;

#pragma region Scaling

// font units × pixelSize / unitsPerEm 在整數中取整。用浮點數計算時，127 × 24 / 1000 這類剛好
// 落在整數附近的值可能被誤差推到另一側，使 bitmap 大小差 1 px。

int scaledFloor(int value, int pixelSize, int unitsPerEm) {
    return static_cast<int>(floorDiv(static_cast<std::int64_t>(value) * pixelSize, unitsPerEm));
}

int scaledCeil(int value, int pixelSize, int unitsPerEm) {
    return static_cast<int>(ceilDiv(static_cast<std::int64_t>(value) * pixelSize, unitsPerEm));
}

int scaledRound(int value, int pixelSize, int unitsPerEm) {
    // round(v) = floor(v + 0.5)，分子分母同乘 2 以維持整數運算。
    const std::int64_t doubled = 2 * static_cast<std::int64_t>(value) * pixelSize;
    return static_cast<int>(floorDiv(doubled + unitsPerEm, 2 * static_cast<std::int64_t>(unitsPerEm)));
}

#pragma endregion  // Scaling

#pragma region Coverage accumulation

// 有號面積累加法（font-rs、stb_truetype v2 的做法）。
//
// 每一格存的不是覆蓋率，而是「這一格與左邊一格的覆蓋率差」。一條邊只影響它經過的幾格，
// 邊右側整排像素的覆蓋率都相同，最後由 resolve() 逐列做前綴和還原。
// 邊往下（+v）為正、往上為負，前綴和即為該像素的繞數（含小數），取絕對值就是 nonzero 規則；
// 重疊的筆畫繞數為 2，夾到 1 之後不會被挖空。
//
// 呼叫端須保證所有座標都在 [0, width] × [0, height] 內。
class CoverageAccumulator {
   public:
    // 每列多留 2 格：位於右緣 x = width 的邊仍會寫到第 width 與 width + 1 格。
    // 這兩格在 bitmap 之外，不會輸出。
    CoverageAccumulator(int width, int height)
        : _width(width),
          _height(height),
          _stride(static_cast<std::size_t>(width) + 2),
          _cells(_stride * static_cast<std::size_t>(height), 0.0f) {}

    void addLine(Vec2 from, Vec2 to) {
        // 水平的邊不穿過任何掃描列，對繞數沒有貢獻。
        if (from.y == to.y) {
            return;
        }

        float direction = 1.0f;
        if (from.y > to.y) {
            std::swap(from, to);
            direction = -1.0f;
        }

        const float dxdy = (to.x - from.x) / (to.y - from.y);
        float x = from.x;

        const int rowStart = std::max(0, static_cast<int>(std::floor(from.y)));
        const int rowEnd = std::min(_height, static_cast<int>(std::ceil(to.y)));

        for (int row = rowStart; row < rowEnd; row++) {
            float* line = _cells.data() + static_cast<std::size_t>(row) * _stride;

            // 邊在這一列內的高度就是這一列要分配的總面積 d（帶方向）。
            const float dy = std::min(static_cast<float>(row + 1), to.y) - std::max(static_cast<float>(row), from.y);
            const float xNext = x + dxdy * dy;
            const float d = dy * direction;

            // 逐列累加 dxdy 的浮點誤差可能讓 x 略為超出 [0, width]，夾回範圍內以免寫到列外。
            const float xa = std::clamp(x, 0.0f, static_cast<float>(_width));
            const float xb = std::clamp(xNext, 0.0f, static_cast<float>(_width));

            const float x0 = std::min(xa, xb);
            const float x1 = std::max(xa, xb);

            const float x0Floor = std::floor(x0);
            const int x0i = static_cast<int>(x0Floor);
            const float x1Ceil = std::ceil(x1);
            const int x1i = static_cast<int>(x1Ceil);

            if (x1i <= x0i + 1) {
                // 邊在這一列只落在一個像素內：該像素位於邊右側的比例是 1 − xMid
                // （xMid 為邊在像素內的平均位置），其餘留給右邊一格，之後的像素都得到完整的 d。
                const float xMid = 0.5f * (xa + xb) - x0Floor;
                line[x0i] += d - d * xMid;
                line[x0i + 1] += d * xMid;
            } else {
                // 邊斜跨多個像素：覆蓋率沿 x 方向從 0 線性增加到 d。
                // 頭尾兩格各是一個三角形（a0、am），中間每格增加 s；最後一格補足剩下的量，
                // 讓整列的總和仍是 d。
                const float s = 1.0f / (x1 - x0);
                const float x0f = x0 - x0Floor;
                const float a0 = 0.5f * s * (1.0f - x0f) * (1.0f - x0f);
                const float x1f = x1 - x1Ceil + 1.0f;
                const float am = 0.5f * s * x1f * x1f;

                line[x0i] += d * a0;

                if (x1i == x0i + 2) {
                    line[x0i + 1] += d * (1.0f - a0 - am);
                } else {
                    const float a1 = s * (1.5f - x0f);  // 到第二格中央為止的累計面積
                    line[x0i + 1] += d * (a1 - a0);

                    for (int column = x0i + 2; column < x1i - 1; column++) {
                        line[column] += d * s;
                    }

                    const float a2 = a1 + static_cast<float>(x1i - x0i - 3) * s;
                    line[x1i - 1] += d * (1.0f - a2 - am);
                }

                line[x1i] += d * am;
            }

            x = xNext;
        }
    }

    std::vector<std::uint8_t> resolve() const {
        std::vector<std::uint8_t> bitmap(static_cast<std::size_t>(_width) * static_cast<std::size_t>(_height));

        for (int row = 0; row < _height; row++) {
            const float* line = _cells.data() + static_cast<std::size_t>(row) * _stride;
            std::uint8_t* output = bitmap.data() + static_cast<std::size_t>(row) * static_cast<std::size_t>(_width);

            float sum = 0.0f;
            for (int column = 0; column < _width; column++) {
                sum += line[column];
                const float coverage = std::min(std::abs(sum), 1.0f);
                output[column] = static_cast<std::uint8_t>(std::lround(coverage * 255.0f));
            }
        }

        return bitmap;
    }

   private:
    int _width;
    int _height;
    std::size_t _stride;
    std::vector<float> _cells;
};

#pragma endregion  // Coverage accumulation

#pragma region Outline tracing

// 把 font units 的輪廓換算成 bitmap 的像素座標，曲線細分成折線後逐段交給 accumulator。
class OutlineTracer {
   public:
    OutlineTracer(CoverageAccumulator& accumulator, float scale, int bearingX, int bearingY, int width, int height)
        : _accumulator(accumulator),
          _scale(scale),
          _bearingX(static_cast<float>(bearingX)),
          _bearingY(static_cast<float>(bearingY)),
          _width(static_cast<float>(width)),
          _height(static_cast<float>(height)) {}

    void moveTo(const GotlData::OutlinePoint& point) {
        close();
        _start = toPixel(point);
        _current = _start;
        _hasContour = true;
    }

    void lineTo(const GotlData::OutlinePoint& point) { lineToPixel(toPixel(point)); }

    void quadTo(const GotlData::OutlinePoint& control, const GotlData::OutlinePoint& end) {
        flattenTo(QuadraticBezier2D{_current, toPixel(control), toPixel(end)});
    }

    void cubicTo(const GotlData::OutlinePoint& control1, const GotlData::OutlinePoint& control2,
                 const GotlData::OutlinePoint& end) {
        flattenTo(CubicBezier2D{_current, toPixel(control1), toPixel(control2), toPixel(end)});
    }

    // GOTL 沒有 Close 指令，輪廓一律封閉；沒閉合的輪廓會讓那一列的累加和不歸零，
    // 右側整排像素都被填滿。
    void close() {
        if (_hasContour) {
            lineToPixel(_start);
        }
        _hasContour = false;
    }

   private:
    // Y 軸翻轉成向下。bbox 包含所有控制點，換算後理論上都在 bitmap 內；
    // 夾到範圍內是為了吸收浮點誤差，也滿足 CoverageAccumulator 的前提。
    Vec2 toPixel(const GotlData::OutlinePoint& point) const {
        const float u = static_cast<float>(point.x) * _scale - _bearingX;
        const float v = _bearingY - static_cast<float>(point.y) * _scale;
        return Vec2(std::clamp(u, 0.0f, _width), std::clamp(v, 0.0f, _height));
    }

    void lineToPixel(const Vec2& point) {
        _accumulator.addLine(_current, point);
        _current = point;
    }

    // 控制點已換算成像素座標，細分的容許誤差因此以像素計，字號越大切得越細。
    template <typename Curve>
    void flattenTo(const Curve& curve) {
        _samples.clear();
        flatten(curve, FLATTEN_TOLERANCE, _samples);

        for (const CurveSample2D& sample : _samples) {
            lineToPixel(sample.position);
        }
    }

    CoverageAccumulator& _accumulator;

    float _scale;
    float _bearingX, _bearingY;
    float _width, _height;

    Vec2 _start;
    Vec2 _current;
    bool _hasContour = false;

    std::vector<CurveSample2D> _samples;  // 重複使用，避免每段曲線都重新配置
};

#pragma endregion  // Outline tracing

}  // namespace

RasterizedGlyph rasterizeGlyph(const GotlData& font, const GotlData::Glyph& glyph, int pixelSize) {
    const int unitsPerEm = font.metrics().unitsPerEm;

    RasterizedGlyph result{};
    result.advanceX = scaledRound(glyph.advanceX, pixelSize, unitsPerEm);

    if (glyph.opCount == 0) {
        return result;
    }

    // bitmap 向外取整到整數像素，確保 bbox 內的輪廓都落在 bitmap 中。
    result.bearingX = scaledFloor(glyph.xMin, pixelSize, unitsPerEm);
    result.bearingY = scaledCeil(glyph.yMax, pixelSize, unitsPerEm);
    result.width = scaledCeil(glyph.xMax, pixelSize, unitsPerEm) - result.bearingX;
    result.height = result.bearingY - scaledFloor(glyph.yMin, pixelSize, unitsPerEm);

    // 例如只有一條水平線的字形，換算後沒有面積。
    if (result.width == 0 || result.height == 0) {
        return result;
    }

    CoverageAccumulator accumulator(result.width, result.height);

    const float scale = static_cast<float>(pixelSize) / static_cast<float>(unitsPerEm);
    OutlineTracer tracer(accumulator, scale, result.bearingX, result.bearingY, result.width, result.height);

    for (const auto& command : font.outline(glyph)) {
        switch (command.op) {
        case GotlData::OutlineOp::MOVE_TO:
            tracer.moveTo(command.points[0]);
            break;
        case GotlData::OutlineOp::LINE_TO:
            tracer.lineTo(command.points[0]);
            break;
        case GotlData::OutlineOp::CURVE_TO:
            if (font.metrics().curveOrder == 2) {
                tracer.quadTo(command.points[0], command.points[1]);
            } else {
                tracer.cubicTo(command.points[0], command.points[1], command.points[2]);
            }
            break;
        }
    }

    tracer.close();  // 輪廓資料結尾也要閉合最後一個輪廓

    result.bitmap = accumulator.resolve();
    return result;
}

}  // namespace robot
