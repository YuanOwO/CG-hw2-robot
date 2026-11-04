#pragma once

#include "math/vec3.hpp"

namespace robot::world::physics {

/// 剛體，具有質量、位置、速度與是否接觸地面等屬性，還不支援旋轉與角速度。
class RigidBody {
    friend class Simulator;

   public:
    explicit RigidBody(float mass, Vec3 position);

    float mass() const { return _mass; }
    float inverseMass() const { return _inverseMass; }

    const Vec3& position() const { return _position; }
    const Vec3& velocity() const { return _velocity; }
    bool onGround() const { return _onGround; }

    void setPosition(const Vec3& position);

   private:
    float _mass, _inverseMass;
    Vec3 _position;
    Vec3 _velocity;
    bool _onGround = false;
};

}  // namespace robot::world::physics
