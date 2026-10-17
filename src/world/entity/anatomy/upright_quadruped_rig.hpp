#pragma once

#include "world/entity/anatomy/quadruped_rig.hpp"

namespace robot::world::entity::anatomy {

class UprightQuadrupedRig : public QuadrupedRig {
   public:
    UprightQuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait);
};

}  // namespace robot::world::entity::anatomy
