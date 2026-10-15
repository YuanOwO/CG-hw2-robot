#include "math/path2d.hpp"

#include <cassert>
#include <cmath>
#include <optional>

#include "math/utils.hpp"

namespace robot {

namespace {

// 兩段相接處切線的夾角在這以內視為平滑。手算的座標與 cos、sin 都有誤差，不能要求完全相等
const float SMOOTH_JOIN_DEGREES = 1.0f;

// 前進方向的右手邊，direction 須為單位向量
Vec2 rightNormal(const Vec2& direction) {
    return Vec2(direction.y, -direction.x);
}

}  // namespace

#pragma region Path2D

Path2D::Path2D(const Vec2& start) : _start(start), _end(start) {}

#pragma region Drawing

Path2D& Path2D::lineTo(const Vec2& end) {
    _segments.push_back(LineSegment2D{_end, end});
    _end = end;
    return *this;
}

Path2D& Path2D::quadTo(const Vec2& control, const Vec2& end) {
    _segments.push_back(QuadraticBezier2D{_end, control, end});
    _end = end;
    return *this;
}

Path2D& Path2D::cubicTo(const Vec2& control1, const Vec2& control2, const Vec2& end) {
    _segments.push_back(CubicBezier2D{_end, control1, control2, end});
    _end = end;
    return *this;
}

Path2D& Path2D::arcAround(const Vec2& center, float sweepDegrees) {
    assert(center != _end && "圓心與目前位置重合，半徑為 0");

    const Arc2D arc = Arc2D::from(_end, center, sweepDegrees);
    _segments.push_back(arc);
    _end = evaluate(arc, 1.0f);
    return *this;
}

Path2D& Path2D::tangentArcTo(const Vec2& end) {
    assert(!_segments.empty() && "第一段沒有可以沿用的切線方向");

    const std::optional<Arc2D> arc = Arc2D::fromTangent(_end, tangent(_segments.back(), 1.0f), end);
    if (!arc) {
        return lineTo(end);
    }

    _segments.push_back(*arc);
    _end = end;  // 直接用指定的終點，不用算出來的，免得之後接上的點有誤差
    return *this;
}

#pragma endregion  // Drawing

#pragma region Flattening

std::vector<std::vector<Path2D::Vertex>> Path2D::flatten(float tolerance) const {
    const float smoothCosine = std::cos(radians(SMOOTH_JOIN_DEGREES));

    std::vector<std::vector<Vertex>> strips;
    Vec2 previousDirection;  // 前一段終點的方向，判斷相接處是否平滑
    std::vector<CurveSample2D> samples;  // 重複使用，避免每段都重新配置

    for (const Segment& segment : _segments) {
        const Vec2 direction = tangent(segment, 0.0f);
        if (direction == Vec2()) {
            continue;  // 長度為 0 的線段沒有方向，也不影響形狀
        }

        // 第一段或折角處開始新的一條；平滑時沿用前一條，接點已經由前一段加過了
        if (strips.empty() || dot(previousDirection, direction) < smoothCosine) {
            strips.push_back({{evaluate(segment, 0.0f), rightNormal(direction)}});
        }

        samples.clear();
        robot::flatten(segment, tolerance, samples);  // 成員函式同名，會遮住自由函式
        for (const CurveSample2D& sample : samples) {
            strips.back().push_back({sample.position, rightNormal(tangent(segment, sample.t))});
        }

        previousDirection = tangent(segment, 1.0f);
    }
    return strips;
}

#pragma endregion  // Flattening

#pragma endregion  // Path2D

#pragma region Segment

Vec2 evaluate(const Path2D::Segment& segment, float t) {
    return std::visit([t](const auto& curve) { return evaluate(curve, t); }, segment);
}

Vec2 derivative(const Path2D::Segment& segment, float t) {
    return std::visit([t](const auto& curve) { return derivative(curve, t); }, segment);
}

Vec2 tangent(const Path2D::Segment& segment, float t) {
    return std::visit([t](const auto& curve) { return tangent(curve, t); }, segment);
}

void flatten(const Path2D::Segment& segment, float tolerance, std::vector<CurveSample2D>& samples) {
    std::visit([&](const auto& curve) { flatten(curve, tolerance, samples); }, segment);
}

#pragma endregion  // Segment

}  // namespace robot
