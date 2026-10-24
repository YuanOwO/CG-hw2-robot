#pragma once

#include <cstdint>
#include <memory>

#include "math/random.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/stance.hpp"
#include "world/collision/collision.hpp"
#include "world/entity/brain/brain.hpp"
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

    const Vec3& position() const { return _position; }  ///< 腳底
    float yaw() const { return _yaw; }                  ///< 身體朝向，方向定義與 Look::yaw 相同
    const Vec3& velocity() const { return _velocity; }
    const Look& look() const { return _look; }
    bool onGround() const { return _onGround; }
    const anatomy::StanceTransition& stance() const { return _stance; }

    /// 世界座標的碰撞體；stance 過渡中依進度內插
    collision::UprightCylinder collider() const;

   protected:
    Entity(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain);

    virtual float mass() const = 0;        /// 質量，單位 kg
    virtual float jumpHeight() const = 0;  /// 起跳高度，單位 m
    virtual float walkSpeed() const = 0;   /// 走路速度，單位 m/s
    virtual float runSpeed() const = 0;    /// 跑步速度，單位 m/s
    float jumpSpeed() const;               /// 起跳速度，單位 m/s

    /// 以腳底為原點的碰撞體
    virtual const collision::UprightCylinder& localCollider(anatomy::Stance stance) const = 0;
    virtual bool supportsStance(anatomy::Stance stance) const = 0;

   private:
    EntityId _id;
    Seed _seed;
    Random _random;
    std::uint32_t _age = 0;

    Vec3 _position;
    float _yaw = 0.0f;
    Vec3 _velocity;
    Look _look;
    bool _onGround = false;
    anatomy::StanceTransition _stance;

    std::unique_ptr<brain::Brain> _brain;

    Motion drift(Motion motion, const World& world, float dt);           /// 受外力影響的速度與位移
    Motion steer(Motion motion, const brain::Intent& intent, float dt);  /// 依 intent 產生速度與位移
    void applyMotion(const MotionResult& motionResult, float dt);
};

}  // namespace robot::world::entity
