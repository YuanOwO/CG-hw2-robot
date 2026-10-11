#include "math/vec2.hpp"

#include <algorithm>
#include <cmath>

namespace robot {

float length(const Vec2& vec) {
    return std::hypot(vec.x, vec.y);
}

float lengthSquared(const Vec2& vec) {
    return dot(vec, vec);
}

float distance(const Vec2& a, const Vec2& b) {
    return length(b - a);
}

float distanceToSegment(const Vec2& point, const Vec2& start, const Vec2& end) {
    const Vec2 segment = end - start;
    const float segmentLengthSquared = lengthSquared(segment);

    if (segmentLengthSquared < EPSILON * EPSILON) {
        return distance(point, start);  // 線段退化成一點
    }

    const float t = std::clamp(dot(point - start, segment) / segmentLengthSquared, 0.0f, 1.0f);
    return distance(point, lerp(start, end, t));
}

float dot(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

float cross(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

Vec2 normalize(const Vec2& vec) {
    float len = length(vec);
    if (len < EPSILON) {
        return Vec2(0.0f, 0.0f);  // 避免除以零
    }
    return vec / len;
}

Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
    return a + (b - a) * t;
}

}  // namespace robot
