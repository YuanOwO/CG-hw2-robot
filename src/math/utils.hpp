#pragma once

#include <cmath>
#include <cstdint>

#include "math/constants.hpp"

namespace robot {

#pragma region Angles

// glRotatef 使用角度，std::sin / std::cos 使用弧度
inline float radians(float degrees) {
    return degrees * (PI / 180.0f);
}

inline float degrees(float radians) {
    return radians * (180.0f / PI);
}

// 換到 [0, 360)
inline float wrapDegrees(float degrees) {
    degrees = std::fmod(degrees, 360.0f);
    if (degrees < 0.0f) {
        degrees += 360.0f;  // fmod 保留被除數的正負號，負角度要再加一圈
    }
    return degrees < 360.0f ? degrees : 0.0f;  // 極小的負數加上 360 會被 float 進位成 360
}

// 換到 (-180, 180]，用來判斷往哪邊轉比較近
inline float wrapSignedDegrees(float degrees) {
    degrees = std::fmod(degrees, 360.0f);
    if (degrees > 180.0f) {
        degrees -= 360.0f;
    } else if (degrees <= -180.0f) {
        degrees += 360.0f;
    }
    return degrees;
}

#pragma endregion  // Angles

#pragma region Interpolation

// t = 0 時為 a，t = 1 時為 b；t 不限制在 [0, 1]，需要時請搭配 std::clamp。
// 適用任何支援 a + (b - a) * t 的型別（float、Vec2、Vec3）
template <typename T>
T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

// 每秒以 rate 的比例縮小與目標的差距時，這一步要走掉差距的多少。
// tick 長短不同時結果仍一致，也不會衝過頭
inline float approach(float rate, float dt) {
    // 差距依指數縮小，連續兩步的結果與一次走完相同
    return 1.0f - std::exp(-rate * dt);
}

#pragma endregion  // Interpolation

#pragma region Integer division

// 向負無窮取整的整數除法（C++ 的 / 向 0 取整）；denominator 必須不為 0
inline std::int64_t floorDiv(std::int64_t numerator, std::int64_t denominator) {
    std::int64_t quotient = numerator / denominator;
    if ((numerator % denominator != 0) && ((numerator < 0) != (denominator < 0))) {
        quotient--;
    }
    return quotient;
}

// 向正無窮取整的整數除法；denominator 必須不為 0
inline std::int64_t ceilDiv(std::int64_t numerator, std::int64_t denominator) {
    return -floorDiv(-numerator, denominator);
}

#pragma endregion  // Integer division

}  // namespace robot
