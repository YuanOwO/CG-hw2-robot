#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace robot {

// 載入後的 GOTL 向量字型。與檔案格式版本無關，由 io::GotlParserV1 等 parser 建立。
// 座標單位為 font units，Y 軸向上，原點為 pen 在基線上的位置。
class GotlFont {
   public:
    struct Metrics {
        int unitsPerEm;
        int ascender;    // baseline 以上的高度
        int descender;   // baseline 以下的深度，為非負值
        int curveOrder;  // 所有 CurveTo 的次數，2 或 3
    };

    struct Glyph {
        char32_t codepoint;

        std::size_t outlineOffset;  // 在 outline data 中的位置
        int opCount;                // 0 表示沒有輪廓（例如空白）

        int advanceX;

        int xMin, yMin, xMax, yMax;  // 包含控制點的 bounding box
    };

    enum class OutlineOp : std::uint8_t {
        MOVE_TO = 0,
        LINE_TO = 1,
        CURVE_TO = 2,
    };

    struct OutlinePoint {
        int x, y;
    };

    // 一個指令與它的點；CurveTo 依 curveOrder 使用前 2 或 3 個點，最後一個為終點。
    struct OutlineCommand {
        OutlineOp op;
        OutlinePoint points[3];
    };

    // glyphs 必須已依 codepoint 嚴格遞增排列，且每個輪廓都通過合法性檢查；
    // 讀檔時由 parser 負責驗證。
    GotlFont(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> outlineData);

    const Metrics& metrics() const { return _metrics; }
    const std::vector<Glyph>& glyphs() const { return _glyphs; }

    const Glyph* findGlyph(char32_t codepoint) const;
    const Glyph* findGlyphOrFallback(char32_t codepoint) const;

    // 解碼字形的輪廓。輪廓的閉合由使用端處理：遇到下一個 MoveTo 與結尾時都要回到起點。
    std::vector<OutlineCommand> outline(const Glyph& glyph) const;

   private:
    Metrics _metrics;
    std::vector<Glyph> _glyphs;
    std::vector<std::uint8_t> _outlineData;
};

}  // namespace robot
