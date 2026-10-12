#include "world/entity/entity.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::entity {

namespace {

const float MAX_PITCH = 90.0f;

}  // namespace

// yaw 為 0 時面向 +z、正值逆時針，因此前方是 (sin yaw, 0, cos yaw)
Vec3 forwardOf(float yaw) {
    const float radiansYaw = radians(yaw);
    return Vec3(std::sin(radiansYaw), 0.0f, std::cos(radiansYaw));
}

Entity::Entity(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float pitch)
    : _id(id), _random(seed), _position(position), _yaw(wrapDegrees(yaw)), _pitch(std::clamp(pitch, -MAX_PITCH, MAX_PITCH)) {}

void Entity::setYaw(float yaw) {
    _yaw = wrapDegrees(yaw);
}

void Entity::setPitch(float pitch) {
    _pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
}

}  // namespace robot::world::entity
