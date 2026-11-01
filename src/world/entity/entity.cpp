#include "world/entity/entity.hpp"

#include <algorithm>
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
    const auto intent = _brain->think(*this, world, dt);

    if (intent.look.has_value()) {
        _look = intent.look.value();
        _yaw = _look.yaw;  // 身體朝向與視線一致
    }

    if (intent.stance.has_value() && supportsStance(intent.stance.value())) {
        _stance.request(intent.stance.value());
    }

    _stance.advance(dt, 0.5f);  // 轉換姿勢的時間固定為 0.5 秒
    updatePose(_pose);

    auto driveInput = steer(intent);
    world.physics().simulate(_rigidBody, collider(), driveInput, dt);

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
