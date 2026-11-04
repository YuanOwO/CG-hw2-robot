#include "geometry/curve/arc.hpp"

#include <cassert>
#include <cmath>

#include "math/constants.hpp"
#include "math/utils.hpp"

namespace robot::geometry {

Arc2D::Arc2D(const Vec2& center, float radius, float startDegree, float sweepDegree)
    : _center(center), _radius(radius), _startDegree(startDegree), _sweepDegree(sweepDegree) {
    assert(radius >= 0.0f);
    assert(std::isfinite(radius));
    assert(std::isfinite(startDegree));
    assert(std::isfinite(sweepDegree));
}

std::optional<Arc2D> Arc2D::around(const Vec2& start, const Vec2& center, float sweepDegree) {
    Vec2 dir = start - center;
    float radius = length(dir);
    if (radius <= EPSILON) {
        return std::nullopt;
    }
    float startDegree = degrees(std::atan2(dir.y, dir.x));
    return Arc2D(center, radius, startDegree, sweepDegree);
}

std::optional<Arc2D> Arc2D::around(const Vec2& start, const Vec2& center, const Vec2& end, ArcDirection direction) {
    Vec2 startDir = start - center;
    Vec2 endDir = end - center;
    float radius = length(startDir);
    if (radius <= EPSILON || length(endDir) <= EPSILON) {
        return std::nullopt;
    }

    float startDegree = degrees(std::atan2(startDir.y, startDir.x));
    float endDegree = degrees(std::atan2(endDir.y, endDir.x));

    if (startDegree == endDegree) {
        // 方向相同時轉一整圈
        return Arc2D(center, radius, startDegree, direction == ArcDirection::Clockwise ? -360.0f : 360.0f);
    }

    float sweepDegree = wrapDegrees(endDegree - startDegree);
    if (direction == ArcDirection::Clockwise) {
        sweepDegree = sweepDegree - 360.0f;
    }
    return Arc2D(center, radius, startDegree, sweepDegree);
}

std::optional<Arc2D> Arc2D::through(const Vec2& start, const Vec2& via, const Vec2& end) {
    Vec2 ab = via - start;
    Vec2 ac = end - start;
    float abLen = length(ab);
    float acLen = length(ac);
    float denom = cross(ab, ac);

    // 三點共線或其中兩點重合
    if (std::abs(denom) <= EPSILON * abLen * acLen) {
        // 三點共線或其中兩點重合
        return std::nullopt;
    }

    // 計算通過三點的圓弧
    float abLenSq = abLen * abLen;
    float acLenSq = acLen * acLen;
    float x = (abLenSq * ac.y - acLenSq * ab.y) / (2.0f * denom);
    float y = (acLenSq * ab.x - abLenSq * ac.x) / (2.0f * denom);
    Vec2 center = start + Vec2(x, y);

    if (denom > 0.0f) {
        // 逆時針方向
        return around(start, center, end, ArcDirection::CounterClockwise);
    } else {
        // 順時針方向
        return around(start, center, end, ArcDirection::Clockwise);
    }
}

std::optional<Arc2D> Arc2D::tangentAtStart(const Vec2& start, const Vec2& direction, const Vec2& end) {
    Vec2 m = normalize(Vec2(-direction.y, direction.x));  // 與 direction 垂直的單位向量
    Vec2 ab = end - start;
    float denom = dot(m, ab);
    if (std::abs(denom) <= EPSILON * length(ab)) {
        // end 落在從 start 沿 direction 延伸的直線上
        return std::nullopt;
    }

    float s = lengthSquared(ab) / (2.0f * denom);
    Vec2 center = start + m * s;
    if (s > 0.0f) {
        // 逆時針方向
        return around(start, center, end, ArcDirection::CounterClockwise);
    } else {
        // 順時針方向
        return around(start, center, end, ArcDirection::Clockwise);
    }
}

std::optional<Arc2D> Arc2D::tangentAtEnd(const Vec2& start, const Vec2& end, const Vec2& direction) {
    Vec2 m = normalize(Vec2(-direction.y, direction.x));  // 與 direction 垂直的單位向量
    Vec2 ba = start - end;
    float denom = dot(m, ba);
    if (std::abs(denom) <= EPSILON * length(ba)) {
        // start 落在通過 end、沿 direction 延伸的直線上
        return std::nullopt;
    }

    float s = lengthSquared(ba) / (2.0f * denom);
    Vec2 center = end + m * s;
    if (s > 0.0f) {
        // 逆時針方向
        return around(start, center, end, ArcDirection::CounterClockwise);
    } else {
        // 順時針方向
        return around(start, center, end, ArcDirection::Clockwise);
    }
}

Vec2 Arc2D::evaluate(float t) const {
    float angle = _startDegree + _sweepDegree * t;
    float rad = radians(angle);
    Vec2 r(_radius * std::cos(rad), _radius * std::sin(rad));
    return _center + r;
}

Vec2 Arc2D::derivative(float t) const {
    if (_radius == 0 || _sweepDegree == 0) {
        return Vec2(0.0f, 0.0f);
    }
    float angle = _startDegree + _sweepDegree * t;
    float rad = radians(angle);
    Vec2 dir(-std::sin(rad), std::cos(rad));
    return dir * (_radius * radians(_sweepDegree));
}

Vec2 Arc2D::tangent(float t) const {
    if (_radius == 0 || _sweepDegree == 0) {
        return Vec2(0.0f, 0.0f);
    }
    float angle = _startDegree + _sweepDegree * t;
    float rad = radians(angle);
    Vec2 dir(-std::sin(rad), std::cos(rad));
    if (_sweepDegree < 0) {
        dir = -dir;  // 順時針方向
    }
    return dir;
}

std::pair<Arc2D, Arc2D> Arc2D::split(float t) const {
    float midDegree = _startDegree + _sweepDegree * t;
    float endDegree = _startDegree + _sweepDegree;
    Arc2D first(_center, _radius, _startDegree, midDegree - _startDegree);
    Arc2D second(_center, _radius, midDegree, endDegree - midDegree);
    return {first, second};
}

bool Arc2D::isFlat(float tolerance) const {
    float halfAngle = std::abs(_sweepDegree) / 2.0f;
    float rad = radians(halfAngle);
    float error = _radius * (1.0f - std::cos(rad));
    return error <= tolerance;
}

}  // namespace robot::geometry
