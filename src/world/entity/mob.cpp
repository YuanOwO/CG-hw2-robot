#include "world/entity/mob.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::entity {

namespace {

const float MIN_TURNING_SPEED_RATIO = 0.2f;  // 轉大彎時仍保留的速度比例，避免停在原地打轉
const float WALK_ANIMATION_RATE = 10.0f;     // 1 / 秒，越大跟得越快；10 約 0.3 秒跟上

}  // namespace

Mob::Mob(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float walkSpeed, float maxTurnRate)
    : Entity(id, seed, position, yaw), _walkSpeed(walkSpeed), _maxTurnRate(maxTurnRate), _targetYaw(this->yaw()) {}

void Mob::tick(float dt, const World& world) {
    aiStep(dt, world);
    travel(dt);
}

void Mob::setMoveIntent(float targetYaw, float speedFactor) {
    _targetYaw = targetYaw;
    _speedFactor = std::clamp(speedFactor, 0.0f, 1.0f);
}

void Mob::travel(float dt) {
    const float maxTurn = _maxTurnRate * dt;
    setYaw(yaw() + std::clamp(wrapSignedDegrees(_targetYaw - yaw()), -maxTurn, maxTurn));

    // 用轉向後剩下的誤差調速：方向還沒對齊時慢慢走，對齊後恢復全速
    const float remaining = std::min(std::abs(wrapSignedDegrees(_targetYaw - yaw())), 90.0f);
    const float alignment = std::max(MIN_TURNING_SPEED_RATIO, std::cos(radians(remaining)));
    _speed = _walkSpeed * _speedFactor * alignment;

    setMotion(forwardOf(yaw()) * _speed);
    move(dt);
    _walkDistance += _speed * dt;

    // 每秒縮小固定比例的差距；用指數而不是 rate * dt，tick 長短不同時結果仍一致，也不會衝過頭
    const float target = _speed / _walkSpeed;
    _walkAnimation += (target - _walkAnimation) * (1.0f - std::exp(-WALK_ANIMATION_RATE * dt));
}

}  // namespace robot::world::entity
