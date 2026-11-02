#pragma once

#include <optional>

#include "world/anatomy/stance.hpp"
#include "world/types.hpp"

namespace robot::world::entity::brain {

struct Intent {
    float surge = 0.0f;                     ///< -1～1，正值往前
    float sway = 0.0f;                      ///< -1～1，正值往右
    bool run = false;                       ///< 用跑步速度移動
    bool jump = false;                      ///< 著地時才會起跳
    std::optional<Look> look;               ///< 沒有時維持原本的視線
    std::optional<anatomy::Stance> stance;  ///< 想換的 stance；物種不支援時維持原本的
};

class Brain {
   public:
    virtual ~Brain() = default;

    virtual Intent think(const Entity& self, const World& world, float dt) = 0;
};

}  // namespace robot::world::entity::brain
