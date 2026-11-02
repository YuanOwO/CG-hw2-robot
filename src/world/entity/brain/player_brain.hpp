#pragma once

#include "world/entity/brain/brain.hpp"

namespace robot::world::entity::brain {

class PlayerBrain final : public Brain {
   public:
    void setIntent(const Intent& intent) { _intent = intent; }

    Intent think(const Entity& self, const World& world, float dt) override {
        Intent intent = _intent;
        _intent = Intent();  // 重置 intent，下一個 tick 沒有新的
        return intent;
    }

   private:
    Intent _intent;
};

}  // namespace robot::world::entity::brain
