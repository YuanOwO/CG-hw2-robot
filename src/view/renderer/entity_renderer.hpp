#pragma once

#include "view/animate/animator.hpp"
#include "view/animate/robot_animator.hpp"
#include "world/entity/entity.hpp"

namespace robot::view {

class EntityRenderer {
   public:
    void render(const world::entity::Entity& entity) const;

   private:
    animate::RobotAnimator _robotAnimator;

    /// 沒有動畫的實體回傳 nullptr，直接畫世界給的姿勢
    const animate::Animator* animatorFor(const world::entity::Entity& entity) const;
};

}  // namespace robot::view
