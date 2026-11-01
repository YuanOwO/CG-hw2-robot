#pragma once

#include "world/anatomy/parts/arm.hpp"
#include "world/anatomy/parts/head.hpp"
#include "world/anatomy/parts/leg.hpp"
#include "world/anatomy/parts/neck.hpp"
#include "world/anatomy/parts/torso.hpp"

namespace robot::world::entity::robot_spec {

anatomy::BipedTorsoSpec robotTorsoSpec();
anatomy::NeckSpec robotNeckSpec();
anatomy::HeadSpec robotHeadSpec();
anatomy::ArmSpec robotArmSpec();
anatomy::LegSpec robotLegSpec();

inline const anatomy::BipedTorsoSpec ROBOT_TORSO_SPEC = robotTorsoSpec();
inline const anatomy::NeckSpec ROBOT_NECK_SPEC = robotNeckSpec();
inline const anatomy::HeadSpec ROBOT_HEAD_SPEC = robotHeadSpec();
inline const anatomy::ArmSpec ROBOT_ARM_SPEC = robotArmSpec();
inline const anatomy::LegSpec ROBOT_LEG_SPEC = robotLegSpec();

}  // namespace robot::world::entity::robot_spec
