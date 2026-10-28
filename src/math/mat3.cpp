#include "math/mat3.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot {

Mat3::Mat3() : m{} {
    for (int i = 0; i < 3; ++i) {
        at(i, i) = 1.0f;
    }
}

Mat3::Mat3(std::initializer_list<float> values) : m{} {
    std::copy_n(values.begin(), std::min(values.size(), 9ul), m);
}

Mat3::Mat3(std::initializer_list<std::initializer_list<float>> mat) : m{} {
    int row = 0;
    for (const auto& r : mat) {
        int col = 0;
        for (const auto& v : r) {
            if (row < 3 && col < 3) {
                at(row, col) = v;
            }
            col++;
        }
        row++;
    }
}

Mat3 Mat3::operator*(const Mat3& rhs) const {
    Mat3 result;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            result.at(row, col) = 0.0f;
            for (int k = 0; k < 3; ++k) {
                result.at(row, col) += at(row, k) * rhs.at(k, col);
            }
        }
    }
    return result;
}

Mat3 translation(const Vec2& offset) {
    Mat3 result;
    result.at(0, 2) = offset.x;
    result.at(1, 2) = offset.y;
    return result;
}

Mat3 rotation(float degree) {
    const float c = std::cos(radians(degree));
    const float s = std::sin(radians(degree));

    Mat3 result;
    result.at(0, 0) = c;
    result.at(0, 1) = -s;
    result.at(1, 0) = s;
    result.at(1, 1) = c;
    return result;
}

Mat3 scale(float sx, float sy) {
    Mat3 result;
    result.at(0, 0) = sx;
    result.at(1, 1) = sy;
    return result;
}

Vec2 transformPoint(const Mat3& mat, const Vec2& point) {
    return transformVector(mat, point) + Vec2(mat.at(0, 2), mat.at(1, 2));
}

Vec2 transformVector(const Mat3& mat, const Vec2& vector) {
    float x = mat.at(0, 0) * vector.x + mat.at(0, 1) * vector.y;
    float y = mat.at(1, 0) * vector.x + mat.at(1, 1) * vector.y;
    return Vec2(x, y);
}

}  // namespace robot
