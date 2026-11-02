#pragma once

#include "view/animate/animator.hpp"

namespace robot::view::animate {

/// 機器人的走路動畫：手腳依移動距離擺動，疊加在站姿或潛行姿勢上
class RobotAnimator final : public Animator {
   public:
    world::anatomy::Pose animate(const world::entity::Entity& entity) const override;
};

}  // namespace robot::view::animate
