#pragma once

#include "world/entity/brain/brain.hpp"

namespace robot::world::entity::brain {

class WalkingBrain final : public Brain {
   public:
    Intent think(const Entity& self, const World& world, float dt) override {
        // 就是往前走，沒有其他動作
        return Intent{1.0f, 0.0f, false, false, std::nullopt, std::nullopt};
    }
};

}  // namespace robot::world::entity::brain
