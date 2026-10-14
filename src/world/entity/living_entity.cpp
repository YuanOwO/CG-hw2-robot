#include "world/entity/living_entity.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "math/utils.hpp"
#include "world/entity/anatomy/pose.hpp"

namespace robot::world::entity {

namespace {

const float WALK_ANIMATION_RATE = 10.0f;  // 1 / 秒，越大跟得越快；10 約 0.3 秒跟上
const float BODY_TURN_RATE = 10.0f;       // 1 / 秒，身體跟上目標朝向的快慢

const Vec3 UP(0.0f, 1.0f, 0.0f);

}  // namespace

LivingEntity::LivingEntity(EntityId id, std::uint64_t seed, const anatomy::Rig& rig, const Vec3& position, float yaw,
                           float walkSpeed)
    : Entity(id, seed, position, yaw), _rig(rig), _walkSpeed(walkSpeed), _bodyYaw(this->yaw()) {}

void LivingEntity::tick(float dt, const World& world) {
    travel(aiStep(dt, world), dt);
}

anatomy::AnimationInput LivingEntity::animationInput() const {
    return {wrapSignedDegrees(yaw() - _bodyYaw), pitch(), _walkDistance, _walkAnimation, true};
}

Mat4 LivingEntity::rootTransform() const {
    return translation(position()) * rotation(_bodyYaw, UP);
}

void LivingEntity::computeBoneTransforms(std::vector<Mat4>& out) const {
    anatomy::Pose pose(_rig.skeleton());
    _rig.animate(animationInput(), pose);
    _rig.skeleton().computeTransforms(pose, rootTransform(), out);
}

Vec3 LivingEntity::eyePosition() const {
    if (_rig.eye() == anatomy::NO_MARKER) {
        return Entity::eyePosition();
    }

    std::vector<Mat4> transforms;
    computeBoneTransforms(transforms);
    return _rig.skeleton().markerPosition(transforms, _rig.eye());
}

void LivingEntity::travel(const MoveInput& input, float dt) {
    float forward = std::clamp(input.forward, -1.0f, 1.0f);
    float strafe = std::clamp(input.strafe, -1.0f, 1.0f);

    // 前進與側移同時按時，斜向不能比單一方向快
    const float amount = std::hypot(forward, strafe);
    if (amount > 1.0f) {
        forward /= amount;
        strafe /= amount;
    }

    // 面向 +z 時右手邊是 -x：右方 = 前方 × 上方
    const float radiansYaw = radians(yaw());
    const Vec3 front(std::sin(radiansYaw), 0.0f, std::cos(radiansYaw));
    const Vec3 right(-std::cos(radiansYaw), 0.0f, std::sin(radiansYaw));
    const Vec3 direction = front * forward + right * strafe;

    setMotion(direction * _walkSpeed);
    move(dt);

    // 步伐跟著實際的水平位移，之後被外力推動時腳也會跟著動
    _speed = std::hypot(motion().x, motion().z);
    _walkDistance += _speed * dt;
    _walkAnimation += (_speed / _walkSpeed - _walkAnimation) * approach(WALK_ANIMATION_RATE, dt);

    turnBody(forward, strafe, dt);
}

void LivingEntity::turnBody(float forward, float strafe, float dt) {
    const float maxTwist = _rig.maxHeadYaw();

    // 沒在走時身體轉回頭的方向；往後退時身體維持朝前，而不是轉過去面向後方
    float offset = 0.0f;
    if (_speed > 0.0f) {
        // 直接用輸入算相對於 yaw 的角度：純側移剛好是 ±90°，由世界座標換算回來會有浮點誤差，
        // 越過 90° 就被當成後退而轉向另一邊
        offset = forward < 0.0f ? degrees(std::atan2(strafe, -forward)) : degrees(std::atan2(-strafe, forward));
    }
    const float target = yaw() + std::clamp(offset, -maxTwist, maxTwist);
    float body = _bodyYaw + wrapSignedDegrees(target - _bodyYaw) * approach(BODY_TURN_RATE, dt);

    // 頭轉得太快時直接拖著身體，夾角才不會超過上限
    body = yaw() + std::clamp(wrapSignedDegrees(body - yaw()), -maxTwist, maxTwist);
    _bodyYaw = wrapDegrees(body);
}

}  // namespace robot::world::entity
