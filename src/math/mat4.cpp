#include "math/mat4.hpp"

#include <cmath>

#include "math/utils.hpp"

namespace robot {

Mat4::Mat4() : m{} {
    for (int i = 0; i < 4; ++i) {
        at(i, i) = 1.0f;
    }
}

Mat4::Mat4(std::initializer_list<float> values) : m{} {
    std::copy_n(values.begin(), std::min(values.size(), 16ul), m);
}

Mat4::Mat4(std::initializer_list<std::initializer_list<float>> mat) : m{} {
    int row = 0;
    for (const auto& r : mat) {
        int col = 0;
        for (const auto& v : r) {
            if (row < 4 && col < 4) {
                at(row, col) = v;
            }
            col++;
        }
        row++;
    }
}

Mat4 Mat4::operator*(const Mat4& rhs) const {
    Mat4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.at(row, col) = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.at(row, col) += at(row, k) * rhs.at(k, col);
            }
        }
    }
    return result;
}

Mat4 translation(const Vec3& offset) {
    Mat4 result;
    result.at(0, 3) = offset.x;
    result.at(1, 3) = offset.y;
    result.at(2, 3) = offset.z;
    return result;
}

Mat4 rotation(float degree, const Vec3& axis) {
    const Vec3 n = normalize(axis);
    const float c = std::cos(radians(degree));
    const float s = std::sin(radians(degree));
    const float t = 1.0f - c;

    Mat4 result;
    result.at(0, 0) = c + n.x * n.x * t;
    result.at(0, 1) = n.x * n.y * t - n.z * s;
    result.at(0, 2) = n.x * n.z * t + n.y * s;

    result.at(1, 0) = n.y * n.x * t + n.z * s;
    result.at(1, 1) = c + n.y * n.y * t;
    result.at(1, 2) = n.y * n.z * t - n.x * s;

    result.at(2, 0) = n.z * n.x * t - n.y * s;
    result.at(2, 1) = n.z * n.y * t + n.x * s;
    result.at(2, 2) = c + n.z * n.z * t;
    return result;
}

Mat4 scale(float sx, float sy, float sz) {
    Mat4 result;
    result.at(0, 0) = sx;
    result.at(1, 1) = sy;
    result.at(2, 2) = sz;
    return result;
}

Mat4 scale(float s) {
    return scale(s, s, s);
}

Vec3 transformPoint(const Mat4& mat, const Vec3& point) {
    return transformVector(mat, point) + Vec3(mat.at(0, 3), mat.at(1, 3), mat.at(2, 3));
}

Vec3 transformVector(const Mat4& mat, const Vec3& vector) {
    float x = mat.at(0, 0) * vector.x + mat.at(0, 1) * vector.y + mat.at(0, 2) * vector.z;
    float y = mat.at(1, 0) * vector.x + mat.at(1, 1) * vector.y + mat.at(1, 2) * vector.z;
    float z = mat.at(2, 0) * vector.x + mat.at(2, 1) * vector.y + mat.at(2, 2) * vector.z;
    return Vec3(x, y, z);
}

}  // namespace robot
