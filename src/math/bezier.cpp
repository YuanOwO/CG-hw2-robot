#include "math/bezier.hpp"

#include "math/line_segment.hpp"
#include "math/utils.hpp"

namespace robot {

namespace {

const int MAX_SUBDIVISION_DEPTH = 16;  // 最多切成 2^16 段，避免 NaN 或容許值為 0 時無限遞迴

// 曲線位於控制點的凸包內，因此控制點到弦的距離都夠小時，曲線可以用弦代替。
bool isFlat(const QuadraticBezier2D& curve, float tolerance) {
    return distance(curve.p1, LineSegment2D{curve.p0, curve.p2}) <= tolerance;
}

bool isFlat(const CubicBezier2D& curve, float tolerance) {
    return distance(curve.p1, LineSegment2D{curve.p0, curve.p3}) <= tolerance &&
           distance(curve.p2, LineSegment2D{curve.p0, curve.p3}) <= tolerance;
}

Vec2 endPoint(const QuadraticBezier2D& curve) {
    return curve.p2;
}

Vec2 endPoint(const CubicBezier2D& curve) {
    return curve.p3;
}

// 自適應細分：不夠平就從中間對半切，兩半各自再檢查，彎曲處自然切得較細。
// t0、t1 為子曲線兩端在原曲線上的參數，對半切開時中點的參數就是兩者的平均
template <typename Curve>
void subdivide(const Curve& curve, float tolerance, int depth, float t0, float t1,
               std::vector<CurveSample2D>& samples) {
    if (depth >= MAX_SUBDIVISION_DEPTH || isFlat(curve, tolerance)) {
        samples.push_back({endPoint(curve), t1});  // 只加終點，起點已經由前一段加過了
        return;
    }

    const float middle = (t0 + t1) / 2.0f;
    const auto [left, right] = split(curve, 0.5f);
    subdivide(left, tolerance, depth + 1, t0, middle, samples);
    subdivide(right, tolerance, depth + 1, middle, t1, samples);
}

}  // namespace

#pragma region Evaluation

// 切分點 B(t) 即為左半段的終點。
Vec2 evaluate(const QuadraticBezier2D& curve, float t) {
    return split(curve, t).first.p2;
}

Vec2 evaluate(const CubicBezier2D& curve, float t) {
    return split(curve, t).first.p3;
}

// n 次貝茲曲線的導數是 n - 1 次的曲線，控制點為相鄰控制點的差乘以 n
Vec2 derivative(const QuadraticBezier2D& curve, float t) {
    return lerp(curve.p1 - curve.p0, curve.p2 - curve.p1, t) * 2.0f;
}

Vec2 derivative(const CubicBezier2D& curve, float t) {
    const QuadraticBezier2D hodograph = {curve.p1 - curve.p0, curve.p2 - curve.p1, curve.p3 - curve.p2};
    return evaluate(hodograph, t) * 3.0f;
}

#pragma endregion  // Evaluation

#pragma region Subdivision

// de Casteljau 演算法：相鄰控制點反覆以 t 內插，各層的第一點與最後一點就是兩段的控制點
std::pair<QuadraticBezier2D, QuadraticBezier2D> split(const QuadraticBezier2D& curve, float t) {
    const Vec2 q0 = lerp(curve.p0, curve.p1, t);
    const Vec2 q1 = lerp(curve.p1, curve.p2, t);

    const Vec2 s = lerp(q0, q1, t);  // 曲線上的點 B(t)

    return {
        {curve.p0, q0, s       },
        {s,        q1, curve.p2}
    };
}

std::pair<CubicBezier2D, CubicBezier2D> split(const CubicBezier2D& curve, float t) {
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

void flatten(const QuadraticBezier2D& curve, float tolerance, std::vector<CurveSample2D>& samples) {
    subdivide(curve, tolerance, 0, 0.0f, 1.0f, samples);
}

void flatten(const CubicBezier2D& curve, float tolerance, std::vector<CurveSample2D>& samples) {
    subdivide(curve, tolerance, 0, 0.0f, 1.0f, samples);
}

#pragma endregion  // Subdivision

}  // namespace robot
