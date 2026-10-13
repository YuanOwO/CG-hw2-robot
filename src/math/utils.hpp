#pragma once

#include <cmath>
#include <cstdint>

#include "math/constants.hpp"

namespace robot {

// glRotatef 使用角度，std::sin / std::cos 使用弧度
inline float radians(float degrees) {
    return degrees * (PI / 180.0f);
}

inline float degrees(float radians) {
    return radians * (180.0f / PI);
}

// 換到 [0, 360)；fmod 保留被除數的正負號，負角度要再加一圈。
// 極小的負數加上 360 會被 float 進位成 360，因此再檢查一次
inline float wrapDegrees(float degrees) {
    degrees = std::fmod(degrees, 360.0f);
    if (degrees < 0.0f) {
        degrees += 360.0f;
    }
    return degrees < 360.0f ? degrees : 0.0f;
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

// t = 0 時為 a，t = 1 時為 b；t 不限制在 [0, 1]，需要時請搭配 std::clamp
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// 每秒以 rate 的比例縮小與目標的差距時，這一步要走掉差距的多少。
// 依指數縮小：tick 長短不同時結果仍一致，也不會衝過頭
inline float approach(float rate, float dt) {
    return 1.0f - std::exp(-rate * dt);
}

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

}  // namespace robot
