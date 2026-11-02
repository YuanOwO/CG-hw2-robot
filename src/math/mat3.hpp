#pragma once

#include <initializer_list>

#include "math/vec2.hpp"

namespace robot {

/// 3x3 變換矩陣，作用在行向量上
struct Mat3 {
    float m[9];  ///< 與 OpenGL 相同的 column-major 排列

    /// 單位矩陣
    Mat3();
    Mat3(std::initializer_list<float> values);
    Mat3(std::initializer_list<std::initializer_list<float>> mat);

    /// 第 row 列、第 col 行的元素
    float& at(int row, int col) { return m[col * 3 + row]; }
    float at(int row, int col) const { return m[col * 3 + row]; }
    const float* data() const { return m; }

    /// 先套用 rhs 再套用 this 的變換
    Mat3 operator*(const Mat3& rhs) const;
    Mat3& operator*=(const Mat3& rhs) { return *this = *this * rhs; }
};

/// 平移 offset 的矩陣
Mat3 translation(const Vec2& offset);
/// 繞原點轉 degree 的矩陣：角度、右手定則
Mat3 rotation(float degree);
/// 縮放 sx, sy 的矩陣
Mat3 scale(float sx, float sy);

/// 變換一個點（w = 1），會受平移影響
Vec2 transformPoint(const Mat3& mat, const Vec2& point);
/// 變換一個向量（w = 0），不受平移影響；不會正規化，長度隨矩陣的縮放改變
Vec2 transformVector(const Mat3& mat, const Vec2& vector);

}  // namespace robot
