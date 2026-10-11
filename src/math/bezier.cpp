#include "math/bezier.hpp"

#include "math/vec2.hpp"

namespace robot {

namespace {

const int MAX_SUBDIVISION_DEPTH = 16;  // 最多切成 2^16 段，避免 NaN 或容許值為 0 時無限遞迴

// 曲線位於控制點的凸包內，因此控制點到弦的距離都夠小時，曲線可以用弦代替。
bool isFlat(const QuadraticBezier& curve, float tolerance) {
    return distanceToSegment(curve.p1, curve.p0, curve.p2) <= tolerance;
}

bool isFlat(const CubicBezier& curve, float tolerance) {
    return distanceToSegment(curve.p1, curve.p0, curve.p3) <= tolerance &&
           distanceToSegment(curve.p2, curve.p0, curve.p3) <= tolerance;
}

Vec2 endPoint(const QuadraticBezier& curve) {
    return curve.p2;
}

Vec2 endPoint(const CubicBezier& curve) {
    return curve.p3;
}

template <typename Curve>
void subdivide(const Curve& curve, float tolerance, int depth, std::vector<Vec2>& vertices) {
    if (depth >= MAX_SUBDIVISION_DEPTH || isFlat(curve, tolerance)) {
        vertices.push_back(endPoint(curve));  // 只加終點，起點已經由前一段加過了
        return;
    }

    const auto [left, right] = split(curve, 0.5f);
    subdivide(left, tolerance, depth + 1, vertices);
    subdivide(right, tolerance, depth + 1, vertices);
}

}  // namespace

// 切分點 B(t) 即為左半段的終點。
Vec2 evaluate(const QuadraticBezier& curve, float t) {
    return split(curve, t).first.p2;
}

Vec2 evaluate(const CubicBezier& curve, float t) {
    return split(curve, t).first.p3;
}

std::pair<QuadraticBezier, QuadraticBezier> split(const QuadraticBezier& curve, float t) {
    const Vec2 q0 = lerp(curve.p0, curve.p1, t);
    const Vec2 q1 = lerp(curve.p1, curve.p2, t);

    const Vec2 s = lerp(q0, q1, t);  // 曲線上的點 B(t)

    return {
        {curve.p0, q0, s       },
        {s,        q1, curve.p2}
    };
}

std::pair<CubicBezier, CubicBezier> split(const CubicBezier& curve, float t) {
    const Vec2 q0 = lerp(curve.p0, curve.p1, t);
    const Vec2 q1 = lerp(curve.p1, curve.p2, t);
    const Vec2 q2 = lerp(curve.p2, curve.p3, t);

    const Vec2 r0 = lerp(q0, q1, t);
    const Vec2 r1 = lerp(q1, q2, t);

    const Vec2 s = lerp(r0, r1, t);  // 曲線上的點 B(t)

    return {
        {curve.p0, q0, r0, s       },
        {s,        r1, q2, curve.p3}
    };
}

void flatten(const QuadraticBezier& curve, float tolerance, std::vector<Vec2>& vertices) {
    subdivide(curve, tolerance, 0, vertices);
}

void flatten(const CubicBezier& curve, float tolerance, std::vector<Vec2>& vertices) {
    subdivide(curve, tolerance, 0, vertices);
}

}  // namespace robot
