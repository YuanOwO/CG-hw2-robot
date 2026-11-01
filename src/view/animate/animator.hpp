#pragma once

#include "world/anatomy/pose.hpp"
#include "world/entity/entity.hpp"

namespace robot::view::animate {

/// 依實體的狀態，在基礎姿勢上疊加純視覺的動作；只影響畫面，不影響碰撞與相機
class Animator {
   public:
    virtual ~Animator() = default;

    virtual world::anatomy::Pose animate(const world::entity::Entity& entity) const = 0;
};

}  // namespace robot::view::animate
