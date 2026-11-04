#include "world/entity/entity.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>

#include "math/utils.hpp"
#include "world/world.hpp"

namespace robot::world::entity {

Entity::Entity(EntityId id, Seed seed, float mass, Vec3 position, const anatomy::Skeleton* skeleton,
               std::unique_ptr<brain::Brain> brain)
    : _id(id),
      _seed(seed),
      _random(seed),
      _rigidBody(mass, position),
      _skeleton(skeleton),
      _pose(_skeleton->boneCount()),
      _brain(std::move(brain)) {
    if (_brain == nullptr) {
        throw std::invalid_argument("Entity requires a brain");
    }
}

void Entity::tick(float dt, const World& world) {
    constexpr float STANCE_TRANSITION_DURATION = 0.5f;  // 轉換姿勢的時間固定為 0.5 秒
    constexpr float MOVE_FACING_RATIO = 0.5f;  // 身體偏向移動方向的比例，0 時一直朝視線，1 時完全朝移動方向
    constexpr float BODY_TURN_RATE = 10.0f;    // 身體朝向收斂到目標的速率 (1/s)，越大轉得越快

    const auto intent = _brain->think(*this, world, dt);

    if (intent.look.has_value()) {
        _look = intent.look.value();
    }

    if (intent.stance.has_value() && supportsStance(intent.stance.value())) {
        _stance.request(intent.stance.value());
    }

    _stance.advance(dt, STANCE_TRANSITION_DURATION);
    updatePose(_pose);

    auto driveInput = steer(intent);
    const Vec3 before = _rigidBody.position();
    world.physics().simulate(_rigidBody, collider(), driveInput, dt);

    Vec3 displacement = _rigidBody.position() - before;
    displacement.y = 0.0f;

    const float moved = intent.surge < 0.0f ? -length(displacement) : length(displacement);
    _gait.advance(moved, walkSpeed(), dt);

    // 有移動輸入時，身體才會慢慢轉向視線並偏向移動方向；站著不動只轉頭
    if (std::abs(intent.surge) > 0.0f || std::abs(intent.sway) > 0.0f) {
        // 往後走時左右反向，讓身體背對移動方向而不是轉過去面對它
        const float sign = intent.surge < 0.0f ? -1.0f : 1.0f;
        float offset = degrees(std::atan2(sign * intent.sway, sign * intent.surge)) * MOVE_FACING_RATIO;
        float delta = wrapSignedDegrees(_look.yaw - offset - _yaw);
        _yaw = wrapDegrees(_yaw + delta * approach(BODY_TURN_RATE, dt));
    }

    _age++;
}

collision::UprightCylinder Entity::collider() const {
    const collision::UprightCylinder& from = localCollider(_stance.from);
    const collision::UprightCylinder& to = localCollider(_stance.to);
    const float t = _stance.progress;

    // localCollider 的 baseCenter 是以腳底為原點的，所以要加上腳底位置
    auto rotatedBaseCenter = lerp(from.baseCenter(), to.baseCenter(), t);
    rotatedBaseCenter = rotateAroundY(rotatedBaseCenter, _yaw);

    collision::UprightCylinder local(rotatedBaseCenter, lerp(from.radius(), to.radius(), t),
                                     lerp(from.height(), to.height(), t));

    return local.translated(_rigidBody.position());  // 轉換到世界座標
}

Vec3 Entity::eyePosition() const {
    const auto& eye = _skeleton->marker(eyeMarker());
    const auto transforms = _skeleton->transforms(_pose);
    const Vec3 local = transformPoint(transforms.at(eye.bone), eye.offset);
    return rotateAroundY(local, _yaw) + _rigidBody.position();
}

physics::DriveInput Entity::steer(const brain::Intent& intent) {
    if (_stance.changing()) {
        // 轉換姿勢時不允許移動
        return {};
    }

    float speed = 0.0f;  // 依 stance 與 intent 計算目標速度

    if (_stance.to == anatomy::Stance::Standing) {
        // 站立姿勢才允許走路或跑步
        speed = intent.run ? runSpeed() : walkSpeed();
    } else if (_stance.to == anatomy::Stance::Sneaking) {
        // 潛行姿勢才允許潛行
        speed = sneakSpeed();
    }

    if (speed <= 0.0f) {
        // 沒有速度就不需要計算方向
        return {};
    }

    Vec3 ahead = rotateAroundY(Vec3(0.0f, 0.0f, 1.0f), _look.yaw);
    Vec3 right = rotateAroundY(Vec3(-1.0f, 0.0f, 0.0f), _look.yaw);

    float surge = std::clamp(intent.surge, -1.0f, 1.0f);
    float sway = std::clamp(intent.sway, -1.0f, 1.0f);
    Vec3 direction = ahead * surge + right * sway;

    // 限制 surge 與 sway 的合成速度不超過 1 單位
    float amount = length(direction);
    if (amount > 1.0f) {
        direction /= amount;
    }

    Vec3 targetVelocity = direction * speed;

    physics::DriveInput driveInput;
    driveInput.targetVelocity = targetVelocity;

    if (intent.jump) {
        driveInput.jumpHeight = jumpHeight();
    }

    return driveInput;
}

}  // namespace robot::world::entity
