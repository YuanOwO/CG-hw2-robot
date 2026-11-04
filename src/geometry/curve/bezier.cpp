#include "geometry/curve/bezier.hpp"

#include "geometry/curve/line_segment.hpp"
#include "math/utils.hpp"

namespace robot::geometry {

#pragma region QuadraticBezier2D

QuadraticBezier2D::QuadraticBezier2D(const Vec2& p0, const Vec2& p1, const Vec2& p2) : _p0(p0), _p1(p1), _p2(p2) {}

Vec2 QuadraticBezier2D::evaluate(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    return lerp(p01, p12, t);
}

Vec2 QuadraticBezier2D::derivative(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    return (p12 - p01) * 2.0f;
}

Vec2 QuadraticBezier2D::tangent(float t) const {
    if (_p0 == _p1 && _p1 == _p2) {
        return Vec2(0.0f, 0.0f);
    }
    if (_p0 == _p1) {
        return normalize(_p2 - _p0);
    }
    if (_p1 == _p2) {
        return normalize(_p2 - _p0);
    }

    return normalize(derivative(t));
}

std::pair<QuadraticBezier2D, QuadraticBezier2D> QuadraticBezier2D::split(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    Vec2 mid = lerp(p01, p12, t);

    QuadraticBezier2D first(_p0, p01, mid);
    QuadraticBezier2D second(mid, p12, _p2);
    return {first, second};
}

bool QuadraticBezier2D::isFlat(float tolerance) const {
    // 以起點到終點的直線段代替這段二次貝茲曲線時，誤差不超過 tolerance
    LineSegment2D chord(_p0, _p2);
    float dist = chord.distance(_p1);
    return dist <= tolerance;
}

#pragma endregion  // QuadraticBezier2D

#pragma region CubicBezier2D
CubicBezier2D::CubicBezier2D(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3)
    : _p0(p0), _p1(p1), _p2(p2), _p3(p3) {}

Vec2 CubicBezier2D::evaluate(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    Vec2 p23 = lerp(_p2, _p3, t);

    Vec2 q0 = lerp(p01, p12, t);
    Vec2 q1 = lerp(p12, p23, t);
    return lerp(q0, q1, t);
}

Vec2 CubicBezier2D::derivative(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    Vec2 p23 = lerp(_p2, _p3, t);

    Vec2 q0 = lerp(p01, p12, t);
    Vec2 q1 = lerp(p12, p23, t);
    return (q1 - q0) * 3.0f;
}

Vec2 CubicBezier2D::tangent(float t) const {
    Vec2 velocity = derivative(t);
    if (length(velocity) > EPSILON) {
        return normalize(velocity);
    }

    // 控制點與端點重合時端點的導數為 0，但曲線仍有方向：取極限後等於端點指向下一個不重合的控制點
    if (t <= 0.0f) {
        for (const Vec2& p : {_p1, _p2, _p3}) {
            if (p != _p0) {
                return normalize(p - _p0);
            }
        }
    } else if (t >= 1.0f) {
        for (const Vec2& p : {_p2, _p1, _p0}) {
            if (p != _p3) {
                return normalize(_p3 - p);
            }
        }
    }

    return Vec2(0.0f, 0.0f);  // 所有點重合，或曲線中間的尖點
}

std::pair<CubicBezier2D, CubicBezier2D> CubicBezier2D::split(float t) const {
    Vec2 p01 = lerp(_p0, _p1, t);
    Vec2 p12 = lerp(_p1, _p2, t);
    Vec2 p23 = lerp(_p2, _p3, t);

    Vec2 q0 = lerp(p01, p12, t);
    Vec2 q1 = lerp(p12, p23, t);

    Vec2 mid = lerp(q0, q1, t);

    CubicBezier2D first(_p0, p01, q0, mid);
    CubicBezier2D second(mid, q1, p23, _p3);
    return {first, second};
}

bool CubicBezier2D::isFlat(float tolerance) const {
    // 以起點到終點的直線段代替這段三次貝茲曲線時，誤差不超過 tolerance
    LineSegment2D chord(_p0, _p3);
    float dist1 = chord.distance(_p1);
    float dist2 = chord.distance(_p2);
    return dist1 <= tolerance && dist2 <= tolerance;
}

#pragma endregion  // CubicBezier2D

}  // namespace robot::geometry
