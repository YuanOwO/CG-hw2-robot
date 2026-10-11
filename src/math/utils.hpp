#pragma once

#include "math/constants.hpp"

namespace robot {

// glRotatef 使用角度，std::sin / std::cos 使用弧度
inline float radians(float degrees) {
    return degrees * (PI / 180.0f);
}

inline float degrees(float radians) {
    return radians * (180.0f / PI);
}

// t = 0 時為 a，t = 1 時為 b；t 不限制在 [0, 1]，需要時請搭配 std::clamp
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

}  // namespace robot
