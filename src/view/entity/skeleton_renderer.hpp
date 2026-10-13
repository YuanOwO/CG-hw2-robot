#pragma once

#include "world/entity/living_entity.hpp"

namespace robot::view::entity {

// 畫出實體目前姿勢的骨架：每根骨頭是一條從關節到末端的線，關節是一個點。
// 只讀 rig，任何有身體的實體都能畫
class SkeletonRenderer {
   public:
    // 呼叫前 MODELVIEW 須為世界座標系
    void render(const world::entity::LivingEntity& entity) const;
};

}  // namespace robot::view::entity
