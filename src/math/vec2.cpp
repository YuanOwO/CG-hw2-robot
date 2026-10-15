#include "math/vec2.hpp"

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

}  // namespace robot
