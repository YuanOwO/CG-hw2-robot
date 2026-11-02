#pragma once

#include "world/entity/brain/brain.hpp"

namespace robot::world::entity::brain {

class IdleBrain final : public Brain {
   public:
    Intent think(const Entity& self, const World& world, float dt) override {
        // 什麼都不做，維持原本的視線與姿勢
        return Intent();
    }
};

}  // namespace robot::world::entity::brain
