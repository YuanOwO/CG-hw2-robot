#include "geometry/curve/line_segment.hpp"

#include <algorithm>

#include "math/utils.hpp"

namespace robot::geometry {

#pragma region LineSegment2D

LineSegment2D::LineSegment2D(const Vec2& start, const Vec2& end) : _start(start), _end(end) {}

Vec2 LineSegment2D::evaluate(float t) const {
    return lerp(_start, _end, t);
}

Vec2 LineSegment2D::derivative(float t) const {
    return _end - _start;
}

Vec2 LineSegment2D::tangent(float t) const {
    return normalize(derivative(t));
}

float LineSegment2D::distance(const Vec2& point) const {
    if (_start == _end) {
        return length(point - _start);
    }

    Vec2 ab = _end - _start;
    Vec2 ap = point - _start;
    auto t = std::clamp(dot(ap, ab) / dot(ab, ab), 0.0f, 1.0f);
    Vec2 closest = evaluate(t);
    return length(point - closest);
}

std::pair<LineSegment2D, LineSegment2D> LineSegment2D::split(float t) const {
    Vec2 mid = evaluate(t);
    LineSegment2D first(_start, mid);
    LineSegment2D second(mid, _end);
    return {first, second};
}

bool LineSegment2D::isFlat(float tolerance) const {
    // 直線段永遠是平的
    return true;
}

#pragma endregion  // LineSegment2D

#pragma region LineSegment3D

LineSegment3D::LineSegment3D(const Vec3& start, const Vec3& end) : _start(start), _end(end) {}

Vec3 LineSegment3D::evaluate(float t) const {
    return lerp(_start, _end, t);
}

Vec3 LineSegment3D::derivative(float t) const {
    return _end - _start;
}

float LineSegment3D::distance(const Vec3& point) const {
    if (_start == _end) {
        return length(point - _start);
    }

    Vec3 ab = _end - _start;
    Vec3 ap = point - _start;
    auto t = std::clamp(dot(ap, ab) / dot(ab, ab), 0.0f, 1.0f);
    Vec3 closest = evaluate(t);
    return length(point - closest);
}

Vec3 LineSegment3D::tangent(float t) const {
    return normalize(derivative(t));
}

std::pair<LineSegment3D, LineSegment3D> LineSegment3D::split(float t) const {
    Vec3 mid = evaluate(t);
    LineSegment3D first(_start, mid);
    LineSegment3D second(mid, _end);
    return {first, second};
}

bool LineSegment3D::isFlat(float tolerance) const {
    // 直線段永遠是平的
    return true;
}

#pragma endregion  // LineSegment3D

}  // namespace robot::geometry
