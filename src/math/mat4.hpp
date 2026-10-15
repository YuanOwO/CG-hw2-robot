#pragma once

#include "math/vec3.hpp"

namespace robot {

/// 4x4 變換矩陣，作用在行向量上
struct Mat4 {
    float m[16];  ///< 與 OpenGL 相同的 column-major 排列，可直接交給 glMultMatrixf

    Mat4();  ///< 單位矩陣

    /// 第 row 列、第 col 行的元素
    float& at(int row, int col) { return m[col * 4 + row]; }
    float at(int row, int col) const { return m[col * 4 + row]; }
    const float* data() const { return m; }

    /// 先套用 rhs 再套用 this 的變換
    Mat4 operator*(const Mat4& rhs) const;
    Mat4& operator*=(const Mat4& rhs) { return *this = *this * rhs; }
};

/// 平移 offset 的矩陣
Mat4 translation(const Vec3& offset);
/// 繞 axis 轉 degrees 的矩陣，與 glRotatef 相同：角度、右手定則
/// @param axis 不必是單位向量
Mat4 rotation(float degrees, const Vec3& axis);

/// 變換一個點（w = 1），會受平移影響
Vec3 transformPoint(const Mat4& mat, const Vec3& point);
/// 變換一個向量（w = 0），不受平移影響；不會正規化，長度隨矩陣的縮放改變
Vec3 transformVector(const Mat4& mat, const Vec3& vector);

}  // namespace robot
