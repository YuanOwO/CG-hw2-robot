#pragma once

#include <cmath>

#include "math/constants.hpp"

namespace robot {

/// 3D 向量，== 比較時容許 EPSILON 的誤差
struct Vec3 {
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    bool operator==(const Vec3& other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON && std::abs(z - other.z) < EPSILON;
    }
    bool operator!=(const Vec3& other) const { return !(*this == other); }

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

    Vec3& operator+=(const Vec3& other) { return *this = *this + other; }
    Vec3& operator-=(const Vec3& other) { return *this = *this - other; }
    Vec3& operator*=(float scalar) { return *this = *this * scalar; }
    Vec3& operator/=(float scalar) { return *this = *this / scalar; }

    Vec3 operator+() const { return *this; }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
};

inline Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}

float length(const Vec3& vec);
float lengthSquared(const Vec3& vec);
float distance(const Vec3& a, const Vec3& b);

float dot(const Vec3& a, const Vec3& b);
Vec3 cross(const Vec3& a, const Vec3& b);  ///< 右手定則：cross(X, Y) = Z

Vec3 normalize(const Vec3& vec);  ///< 單位向量；長度小於 EPSILON 時回傳零向量

}  // namespace robot
