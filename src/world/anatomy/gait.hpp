#pragma once

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::anatomy {

/// 走路的進度；畫面依此擺動手腳，同樣的移動距離就是同樣的步伐
struct Gait {
    float distance = 0.0f;  ///< 著地時累積的水平移動距離，m；後退時減少
    float amount = 0.0f;    ///< 擺動幅度倍率，0 表示靜止
    float running = 0.0f;   ///< 跑步的程度 [0, 1]，0 是走路以下的速度

    /// @param moved 這個 tick 的水平移動距離，後退為負值
    /// @param walkSpeed 走路速度，擺動倍率在這個速度下為 1
    void advance(float moved, float walkSpeed, float dt) {
        constexpr float MAX_AMOUNT = 1.2f;  // 再大髖關節往後就會超出關節限制
        constexpr float AMOUNT_RATE = 8.0f;
        constexpr float RUNNING_RATE = 6.0f;

        distance += moved;

        float speed = dt > 0.0f ? std::abs(moved) / dt : 0.0f;

        float targetAmount = std::min(speed / walkSpeed, MAX_AMOUNT);
        amount += (targetAmount - amount) * approach(AMOUNT_RATE, dt);

        // 比走路快一倍才算完全在跑，介於中間時逐漸過渡
        float targetRunning = std::clamp(speed / walkSpeed - 1.0f, 0.0f, 1.0f);
        running += (targetRunning - running) * approach(RUNNING_RATE, dt);
    }
};

}  // namespace robot::world::anatomy
