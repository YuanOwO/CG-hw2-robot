#include "world/entity/chicken.hpp"

#include "common/units.hpp"
#include "world/entity/anatomy/bird_rig.hpp"

namespace robot::world::entity {

namespace {

const float CHICKEN_WALK_SPEED = 0.6f;       // 公尺 / 秒
const float CHICKEN_MAX_TURN_RATE = 120.0f;  // 度 / 秒

// clang-format off
const anatomy::BirdSize CHICKEN_SIZE = {
    {
        Vec3(6.0f, 6.0f, 8.0f) * PX,  // body
        Vec3(4.0f, 6.0f, 3.0f) * PX,  // head
        Vec3(1.0f, 2.5f, 1.0f) * PX,  // upperLeg：小腿
        Vec3(1.0f, 2.5f, 1.0f) * PX,  // lowerLeg：跗蹠骨
        3.0f * PX,                    // hipWidth
    },
    Vec3(1.0f, 4.0f, 6.0f) * PX,   // wing
    Vec3(0.0f, -2.0f, 4.5f) * PX,  // neck：頭沉進軀幹 2 px，凸出前端 2 px
};
const anatomy::BirdGait CHICKEN_GAIT = {
    {
        0.4f,  // strideLength
        35.0f,  // swing
        40.0f,  // kneeBend
        anatomy::KneeDirection::Forward,
    },
    40.0f,  // wingFlap
};
// clang-format on

}  // namespace

const anatomy::Rig& Chicken::speciesRig() {
    static const anatomy::BirdRig rig(CHICKEN_SIZE, CHICKEN_GAIT);
    return rig;
}

Chicken::Chicken(EntityId id, std::uint64_t seed, const Vec3& position, float yaw)
    : Mob(id, seed, speciesRig(), position, yaw, CHICKEN_WALK_SPEED, CHICKEN_MAX_TURN_RATE) {}

void Chicken::think(float dt, const World& /*world*/) {
    // 先隨便走，之後再加上 AI
    steerToward(yaw() + 40.0f * dt, 1.0f);
}

}  // namespace robot::world::entity
