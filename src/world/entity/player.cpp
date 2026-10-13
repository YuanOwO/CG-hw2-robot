#include "world/entity/player.hpp"

#include "common/units.hpp"
#include "math/utils.hpp"
#include "world/entity/anatomy/humanoid_rig.hpp"

namespace robot::world::entity {

namespace {

const float PLAYER_WALK_SPEED = 1.5f;  // 公尺 / 秒
const float LOOK_TURN_RATE = 15.0f;    // 1 / 秒，頭轉向想看的方向的快慢

// clang-format off
const anatomy::HumanoidSize PLAYER_SIZE = {
    {
        Vec3(8.0f, 12.0f, 4.0f) * PX,  // body
        Vec3(8.0f, 8.0f, 8.0f) * PX,   // head
        Vec3(4.0f, 6.0f, 4.0f) * PX,   // upperLeg
        Vec3(4.0f, 6.0f, 4.0f) * PX,   // lowerLeg
    },
    Vec3(4.0f, 6.0f, 4.0f) * PX,  // upperArm
    Vec3(4.0f, 6.0f, 4.0f) * PX,  // lowerArm
};
const anatomy::HumanoidGait PLAYER_GAIT = {
    {
        1.5f,   // strideLength
        30.0f,  // swing
        40.0f,  // kneeBend
    },
    25.0f,  // armSwing
    10.0f,  // elbowRest
    25.0f,  // elbowBend
};
// clang-format on

}  // namespace

const anatomy::Rig& Player::speciesRig() {
    static const anatomy::HumanoidRig rig(PLAYER_SIZE, PLAYER_GAIT);
    return rig;
}

Player::Player(EntityId id, std::uint64_t seed, const Vec3& position, float yaw)
    : LivingEntity(id, seed, speciesRig(), position, yaw, PLAYER_WALK_SPEED) {}

// 頭轉向想看的方向，身體再由 LivingEntity 帶著跟上；移動相對於頭的朝向
MoveInput Player::aiStep(float dt, const World& world) {
    if (_command.look) {
        const float t = approach(LOOK_TURN_RATE, dt);
        auto [lookYaw, lookPitch] = *_command.look;
        setYaw(yaw() + wrapSignedDegrees(lookYaw - yaw()) * t);
        setPitch(pitch() + (lookPitch - pitch()) * t);
    }
    return {_command.forward, _command.strafe};
}

}  // namespace robot::world::entity
