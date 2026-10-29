#include "world/entity/entity.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>

#include "math/utils.hpp"
#include "world/physics/constants.hpp"
#include "world/world.hpp"

namespace robot::world::entity {

Entity::Entity(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain)
    : _id(id), _seed(seed), _random(seed), _position(position), _brain(std::move(brain)) {
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

    physics::Motion motion;
    motion.velocity = _velocity;  // 初速度為目前速度
    motion = drift(motion, world, dt);
    motion = steer(motion, intent, dt);

    motion.displacement = motion.velocity * dt;  // 位移 = 速度 * 時間
    auto result = world.physics().resolve(collider(), motion, _onGround);
    applyMotion(result, dt);

    _age++;
}

collision::UprightCylinder Entity::collider() const {
    const collision::UprightCylinder& from = localCollider(_stance.from);
    const collision::UprightCylinder& to = localCollider(_stance.to);
    const float t = _stance.progress;

    // localCollider 的 baseCenter 是以腳底為原點的，所以要加上腳底位置
    collision::UprightCylinder local(lerp(from.baseCenter(), to.baseCenter(), t), lerp(from.radius(), to.radius(), t),
                                     lerp(from.height(), to.height(), t));
    return local.translated(_position);
}

float Entity::jumpSpeed() const {
    return std::sqrt(2.0f * physics::GRAVITY * jumpHeight());
}

physics::Motion Entity::drift(physics::Motion motion, const World& world, float dt) {
    // 受重力影響
    if (!_onGround) {
        motion.velocity.y -= physics::GRAVITY * dt;
    }

    return motion;
}

physics::Motion Entity::steer(physics::Motion motion, const brain::Intent& intent, float dt) {
    if (_stance.changing()) {
        // 轉換姿勢時不允許移動
        return motion;
    }

    if (_stance.to != anatomy::Stance::Standing) {
        // 非站立姿勢不允許移動
        return motion;
    }

    float radYaw = radians(_look.yaw);
    Vec3 ahead(std::sin(radYaw), 0.0f, std::cos(radYaw));
    Vec3 right(-std::cos(radYaw), 0.0f, std::sin(radYaw));

    float surge = std::clamp(intent.surge, -1.0f, 1.0f);
    float sway = std::clamp(intent.sway, -1.0f, 1.0f);
    Vec3 direction = ahead * surge + right * sway;

    // 限制 surge 與 sway 的合成速度不超過 1 單位
    float amount = length(direction);
    if (amount > 1.0f) {
        direction /= amount;
    }

    float speed = intent.sprint ? runSpeed() : walkSpeed();

    Vec3 targetVelocity = direction * speed;

    if (_onGround) {
        // 先假設地面摩擦力足夠大，能立即將速度調整到目標速度
        motion.velocity.x = targetVelocity.x;
        motion.velocity.z = targetVelocity.z;

        if (intent.jump) {
            motion.velocity.y = jumpSpeed();
        }
    }

    return motion;
}

void Entity::applyMotion(const physics::MotionResult& motionResult, float dt) {
    _position += motionResult.motion.displacement;
    _velocity = motionResult.motion.velocity;
    _onGround = motionResult.onGround;
}

}  // namespace robot::world::entity
