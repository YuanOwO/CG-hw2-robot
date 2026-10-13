#include "world/entity/entity.hpp"

#include <algorithm>

#include "math/utils.hpp"

namespace robot::world::entity {

namespace {

const float MAX_PITCH = 90.0f;

}  // namespace

Entity::Entity(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float pitch, float eyeHeight)
    : _id(id),
      _random(seed),
      _position(position),
      _yaw(wrapDegrees(yaw)),
      _pitch(std::clamp(pitch, -MAX_PITCH, MAX_PITCH)),
      _eyeHeight(eyeHeight) {}

void Entity::setYaw(float yaw) {
    _yaw = wrapDegrees(yaw);
}

void Entity::setPitch(float pitch) {
    _pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
}

}  // namespace robot::world::entity
