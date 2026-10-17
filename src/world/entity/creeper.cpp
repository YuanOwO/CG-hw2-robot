#include "world/entity/creeper.hpp"

#include "common/units.hpp"
#include "world/entity/anatomy/upright_quadruped_rig.hpp"

namespace robot::world::entity {

namespace {

const float CREEPER_WALK_SPEED = 1.0f;      // 公尺 / 秒
const float CREEPER_MAX_TURN_RATE = 90.0f;  // 度 / 秒

const anatomy::QuadrupedSize CREEPER_SIZE = {
    Vec3(8.0f, 12.0f, 4.0f) * PX,  // body
    Vec3(8.0f, 8.0f, 8.0f) * PX,   // head
    Vec3(4.0f, 6.0f, 4.0f) * PX,   // upperLeg
    {}                             // lowerLeg
};
const anatomy::QuadrupedGait CREEPER_GAIT = {1.5f, 30.0f, 0.0f, 0.0f};

}  // namespace

const anatomy::Rig& Creeper::speciesRig() {
    static const anatomy::UprightQuadrupedRig rig(CREEPER_SIZE, CREEPER_GAIT);
    return rig;
}

Creeper::Creeper(EntityId id, std::uint64_t seed, const Vec3& position, float yaw)
    : Mob(id, seed, speciesRig(), position, yaw, CREEPER_WALK_SPEED, CREEPER_MAX_TURN_RATE) {}

void Creeper::think(float dt, const World& /*world*/) {
    // 先隨便走，之後再加上 AI
    steerToward(yaw() + 30.0f * dt, 1.0f);
}

}  // namespace robot::world::entity
