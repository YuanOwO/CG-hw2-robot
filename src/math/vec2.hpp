#pragma once

#include <cmath>

#include "math/constants.hpp"

namespace robot {

/// 2D 向量，== 比較時容許 EPSILON 的誤差
struct Vec2 {
    float x, y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}

    bool operator==(const Vec2& other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    bool operator!=(const Vec2& other) const { return !(*this == other); }

    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

    Vec2& operator+=(const Vec2& other) { return *this = *this + other; }
    Vec2& operator-=(const Vec2& other) { return *this = *this - other; }
    Vec2& operator*=(float scalar) { return *this = *this * scalar; }
    Vec2& operator/=(float scalar) { return *this = *this / scalar; }

    Vec2 operator+() const { return *this; }
    Vec2 operator-() const { return Vec2(-x, -y); }
};

inline Vec2 operator*(float scalar, const Vec2& vec) {
    return vec * scalar;
}

float length(const Vec2& vec);
float lengthSquared(const Vec2& vec);
float distance(const Vec2& a, const Vec2& b);

float dot(const Vec2& a, const Vec2& b);
float cross(const Vec2& a, const Vec2& b);  ///< 2D 外積為純量，即 3D 外積的 z 分量

Vec2 normalize(const Vec2& vec);  ///< 單位向量；長度小於 EPSILON 時回傳零向量

}  // namespace robot
