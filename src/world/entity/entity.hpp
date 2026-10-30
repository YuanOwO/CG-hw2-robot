#pragma once

#include <cstdint>
#include <memory>

#include "math/random.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/stance.hpp"
#include "world/collision/collider.hpp"
#include "world/entity/brain/brain.hpp"
#include "world/physics/motion.hpp"
#include "world/physics/rigid_body.hpp"
#include "world/types.hpp"

namespace robot::world::entity {

class Entity {
   public:
    virtual ~Entity() = default;

    void tick(float dt, const World& world);

    EntityId id() const { return _id; }
    Seed seed() const { return _seed; }
    const Random& random() const { return _random; }
    std::uint32_t age() const { return _age; }  ///< 已經走過幾個 tick

    float yaw() const { return _yaw; }  ///< 身體朝向，方向定義與 Look::yaw 相同
    const Look& look() const { return _look; }

    const Vec3& position() const { return _rigidBody.position(); }
    const Vec3& velocity() const { return _rigidBody.velocity(); }
    bool onGround() const { return _rigidBody.onGround(); }

    const anatomy::StanceTransition& stance() const { return _stance; }

    /// 世界座標的碰撞箱；stance 過渡中依進度內插
    collision::UprightCylinder collider() const;

   protected:
    Entity(EntityId id, Seed seed, float mass, Vec3 position, std::unique_ptr<brain::Brain> brain);

    virtual float jumpHeight() const = 0;  /// 起跳高度，單位 m
    virtual float sneakSpeed() const = 0;  /// 潛行速度，單位 m/s
    virtual float walkSpeed() const = 0;   /// 走路速度，單位 m/s
    virtual float runSpeed() const = 0;    /// 跑步速度，單位 m/s

    /// 以腳底為原點的碰撞箱
    virtual collision::UprightCylinder localCollider(anatomy::Stance stance) const = 0;
    virtual bool supportsStance(anatomy::Stance stance) const = 0;

   private:
    EntityId _id;
    Seed _seed;
    Random _random;
    std::uint32_t _age = 0;

    float _yaw = 0.0f;
    Look _look;
    anatomy::StanceTransition _stance;
    physics::RigidBody _rigidBody;

    std::unique_ptr<brain::Brain> _brain;

    /// 依 intent 計算目標速度
    physics::DriveInput steer(const brain::Intent& intent);
};

}  // namespace robot::world::entity
