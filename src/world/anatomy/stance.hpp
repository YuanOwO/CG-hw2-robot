#pragma once

#include <cstddef>

namespace robot::world::anatomy {

enum class Stance {
    Standing,
    Kneeling,
    Sitting,

    Count
};

constexpr std::size_t STANCE_COUNT = static_cast<std::size_t>(Stance::Count);

struct StanceTransition {
    Stance from = Stance::Standing;
    Stance to = Stance::Standing;
    float progress = 1.0f;  /// [0, 1]

    bool changing() const { return from != to && progress < 1.0f; }
};

}  // namespace robot::world::anatomy
