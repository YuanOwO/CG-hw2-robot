#include "world/physics/rigid_body.hpp"

#include <stdexcept>

namespace robot::world::physics {

RigidBody::RigidBody(float mass, Vec3 position)
    : _mass(mass), _inverseMass(mass > 0.0f ? 1.0f / mass : 0.0f), _position(position) {
    if (mass < 0.0f) {
        throw std::invalid_argument("RigidBody mass must be non-negative");
    }
}

void RigidBody::setPosition(const Vec3& position) {
    _position = position;
}

}  // namespace robot::world::physics
