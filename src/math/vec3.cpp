#include "math/vec3.hpp"

#include <cmath>

namespace robot {

float length(const Vec3& vec) {
    return std::sqrt(lengthSquared(vec));
}

float lengthSquared(const Vec3& vec) {
    return dot(vec, vec);
}

float distance(const Vec3& a, const Vec3& b) {
    return length(b - a);
}

float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Vec3 normalize(const Vec3& vec) {
    float len = length(vec);
    if (len < EPSILON) {
        return Vec3(0.0f, 0.0f, 0.0f);  // 避免除以零
    }
    return vec / len;
}

}  // namespace robot
