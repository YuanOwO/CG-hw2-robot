#pragma once

#include "math/vec3.hpp"

namespace robot {

struct Mat4 {
    float m[16];  // 沿用 OpenGL 的 column-major 排列方式

    Mat4();  // 單位矩陣

    float& at(int row, int col) { return m[col * 4 + row]; }
    float at(int row, int col) const { return m[col * 4 + row]; }
    const float* data() const { return m; }

    Mat4 operator*(const Mat4& rhs) const;
    Mat4& operator*=(const Mat4& rhs) { return *this = *this * rhs; }
};

Mat4 translation(const Vec3& offset);
Mat4 rotation(float degrees, const Vec3& axis);  // 與 glRotatef 相同：角度、右手定則，axis 不必是單位向量

Vec3 transformPoint(const Mat4& mat, const Vec3& point);          // w = 1，受平移影響
Vec3 transformVector(const Mat4& mat, const Vec3& vector);  // w = 0，不受平移影響

}  // namespace robot
