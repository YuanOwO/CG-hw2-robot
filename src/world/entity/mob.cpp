#include "world/entity/mob.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::entity {

namespace {

const float MIN_TURNING_SPEED_RATIO = 0.2f;  // 轉大彎時仍保留的速度比例，避免停在原地打轉

}  // namespace

Mob::Mob(EntityId id, std::uint64_t seed, const anatomy::Rig& rig, const Vec3& position, float yaw, float walkSpeed,
         float maxTurnRate)
    : LivingEntity(id, seed, rig, position, yaw, walkSpeed), _maxTurnRate(maxTurnRate), _targetYaw(this->yaw()) {}

void Mob::steerToward(float targetYaw, float speedFactor) {
    _targetYaw = targetYaw;
    _speedFactor = std::clamp(speedFactor, 0.0f, 1.0f);
}

MoveInput Mob::aiStep(float dt, const World& world) {
    think(dt, world);

    const float maxTurn = _maxTurnRate * dt;
    setYaw(yaw() + std::clamp(wrapSignedDegrees(_targetYaw - yaw()), -maxTurn, maxTurn));

    // 用轉向後剩下的誤差調速：方向還沒對齊時慢慢走，對齊後恢復全速
    const float remaining = std::min(std::abs(wrapSignedDegrees(_targetYaw - yaw())), 90.0f);
    return {_speedFactor * std::max(MIN_TURNING_SPEED_RATIO, std::cos(radians(remaining))), 0.0f};
}

}  // namespace robot::world::entity
