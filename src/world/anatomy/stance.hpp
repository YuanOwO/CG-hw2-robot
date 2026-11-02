#pragma once

#include <cstddef>

#include "math/constants.hpp"

namespace robot::world::anatomy {

enum class Stance {
    Standing,
    Sneaking,
    Sitting,

    Count
};

constexpr std::size_t STANCE_COUNT = static_cast<std::size_t>(Stance::Count);

struct StanceTransition {
    Stance from = Stance::Standing;
    Stance to = Stance::Standing;
    float progress = 1.0f;  /// [0, 1]

    bool changing() const { return from != to; }

    void request(Stance target) {
        if (target == to) {
            return;
        }

        if (!changing()) {
            from = to;
            to = target;
            progress = 0.0f;
        } else if (target == from) {
            // 反轉
            std::swap(from, to);
            progress = 1.0f - progress;
        }

        // 如果 target 既不是 from 也不是 to，則忽略該請求，直到完成當前的轉換
    }

    void advance(float dt, float duration) {
        if (!changing()) {
            return;
        }

        progress = duration > EPSILON ? progress + dt / duration : 1.0f;

        if (progress >= 1.0f) {
            progress = 1.0f;
            from = to;
        }
    }
};

}  // namespace robot::world::anatomy
