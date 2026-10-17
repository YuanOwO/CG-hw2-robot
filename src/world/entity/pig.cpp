#include "world/entity/pig.hpp"

#include "common/units.hpp"
#include "world/entity/anatomy/horizontal_quadruped_rig.hpp"

namespace robot::world::entity {

namespace {

const float PIG_WALK_SPEED = 0.8f;      // 公尺 / 秒
const float PIG_MAX_TURN_RATE = 90.0f;  // 度 / 秒

const anatomy::QuadrupedSize PIG_SIZE = {
    Vec3(10.0f, 8.0f, 16.0f) * PX,  // body
    Vec3(8.0f, 8.0f, 8.0f) * PX,    // head
    Vec3(4.0f, 3.0f, 4.0f) * PX,    // upperLeg
    Vec3(4.0f, 3.0f, 4.0f) * PX,    // lowerLeg
};
const anatomy::QuadrupedGait PIG_GAIT = {1.0f, 30.0f, 30.0f, 30.0f};

}  // namespace

const anatomy::Rig& Pig::speciesRig() {
    static const anatomy::HorizontalQuadrupedRig rig(PIG_SIZE, PIG_GAIT);
    return rig;
}

Pig::Pig(EntityId id, std::uint64_t seed, const Vec3& position, float yaw)
    : Mob(id, seed, speciesRig(), position, yaw, PIG_WALK_SPEED, PIG_MAX_TURN_RATE) {}

void Pig::think(float dt, const World& /*world*/) {
    // 先隨便走，之後再加上 AI；和苦力怕反方向繞圈
    steerToward(yaw() - 20.0f * dt, 1.0f);
}

}  // namespace robot::world::entity
