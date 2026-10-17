#pragma once

#include "world/entity/anatomy/quadruped_rig.hpp"

namespace robot::world::entity::anatomy {

// 軀幹水平，body.z 為身長
class HorizontalQuadrupedRig : public QuadrupedRig {
   public:
    HorizontalQuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait);
};

}  // namespace robot::world::entity::anatomy
