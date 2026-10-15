#include "math/path2d.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "math/utils.hpp"

namespace robot {

namespace {

// 兩段相接處切線的夾角在這以內視為平滑。手算的座標與 cos、sin 都有誤差，不能要求完全相等
const float SMOOTH_JOIN_DEGREES = 1.0f;

// 線段在 t 處前進的方向（未正規化），一般就是導數
template <typename Segment>
Vec2 directionAt(const Segment& segment, float t) {
    return derivative(segment, t);
}

// 控制點與端點重合時，端點的導數是零向量，但曲線在那裡仍有方向：
// 取極限後等於端點指向下一個不重合的控制點。曲線中間的尖點則以前後兩點的差近似
template <typename Curve>
Vec2 bezierDirectionAt(const Curve& curve, float t, const Vec2* points, int count) {
    const Vec2 tangent = derivative(curve, t);
    if (lengthSquared(tangent) > EPSILON * EPSILON) {
        return tangent;
    }

    if (t <= 0.0f) {
        for (int i = 1; i < count; ++i) {
            if (points[i] != points[0]) {
                return points[i] - points[0];
            }
        }
    } else if (t >= 1.0f) {
        for (int i = count - 2; i >= 0; --i) {
            if (points[i] != points[count - 1]) {
                return points[count - 1] - points[i];
            }
        }
    } else {
        const float step = 1e-3f;
        return evaluate(curve, std::min(t + step, 1.0f)) - evaluate(curve, std::max(t - step, 0.0f));
    }
    return Vec2();  // 所有點重合，線段長度為 0
}

Vec2 directionAt(const QuadraticBezier2D& curve, float t) {
    const Vec2 points[] = {curve.p0, curve.p1, curve.p2};
    return bezierDirectionAt(curve, t, points, 3);
}

Vec2 directionAt(const CubicBezier2D& curve, float t) {
    const Vec2 points[] = {curve.p0, curve.p1, curve.p2, curve.p3};
    return bezierDirectionAt(curve, t, points, 4);
}

Vec2 startDirection(const Path2D::Segment& segment) {
    return std::visit([](const auto& s) { return directionAt(s, 0.0f); }, segment);
}

Vec2 endDirection(const Path2D::Segment& segment) {
    return std::visit([](const auto& s) { return directionAt(s, 1.0f); }, segment);
}

// 前進方向的右手邊
Vec2 rightNormal(const Vec2& direction) {
    return normalize(Vec2(direction.y, -direction.x));
}

}  // namespace

Path2D::Path2D(const Vec2& start) : _start(start), _end(start) {}

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

// 與某方向相切且通過兩點的圓只有一個。圓心 C 在通過目前位置 P、垂直於切線的直線上：C = P + n * s，n 為切線左手邊的單位向量。
// 由 |C - P| = |C - E| 解出 s = |E - P|² / (2 n·(E - P))；s 為正時圓心在左手邊，逆時針轉
Path2D& Path2D::tangentArcTo(const Vec2& end) {
    assert(!_segments.empty() && "第一段沒有可以沿用的切線方向");

    const Vec2 tangent = normalize(endDirection(_segments.back()));
    const Vec2 left(-tangent.y, tangent.x);
    const Vec2 chord = end - _end;

    const float offset = dot(left, chord);
    if (std::abs(offset) <= EPSILON * std::max(1.0f, length(chord))) {
        return lineTo(end);
    }

    const Vec2 center = _end + left * (lengthSquared(chord) / (2.0f * offset));

    // 起點與終點相對於圓心的夾角，再依轉的方向換成 (0, 360) 或 (-360, 0)
    const Vec2 from = _end - center;
    const Vec2 to = end - center;
    float sweep = degrees(std::atan2(cross(from, to), dot(from, to)));
    if (offset > 0.0f && sweep < 0.0f) {
        sweep += 360.0f;
    } else if (offset < 0.0f && sweep > 0.0f) {
        sweep -= 360.0f;
    }

    _segments.push_back(Arc2D::from(_end, center, sweep));
    _end = end;  // 直接用指定的終點，不用算出來的，免得之後接上的點有誤差
    return *this;
}

std::vector<std::vector<Path2D::Sample>> Path2D::flatten(float tolerance) const {
    const float smoothCosine = std::cos(radians(SMOOTH_JOIN_DEGREES));

    std::vector<std::vector<Sample>> strips;
    Vec2 previousDirection;  // 前一段終點的方向，判斷相接處是否平滑

    for (const Segment& segment : _segments) {
        const Vec2 direction = normalize(startDirection(segment));
        if (direction == Vec2()) {
            continue;  // 長度為 0 的線段沒有方向，也不影響形狀
        }

        // 第一段或折角處開始新的一條；平滑時沿用前一條，接點已經由前一段加過了
        if (strips.empty() || dot(previousDirection, direction) < smoothCosine) {
            const Vec2 start = std::visit([](const auto& s) { return evaluate(s, 0.0f); }, segment);
            strips.push_back({{start, rightNormal(direction)}});
        }

        std::visit(
            [&](const auto& s) {
                std::vector<CurveSample2D> samples;
                robot::flatten(s, tolerance, samples);  // 成員函式同名，會遮住自由函式
                for (const CurveSample2D& sample : samples) {
                    strips.back().push_back({sample.position, rightNormal(directionAt(s, sample.t))});
                }
            },
            segment);

        previousDirection = normalize(endDirection(segment));
    }
    return strips;
}

}  // namespace robot
