#include "world/entity/robot.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace robot::world::entity {

namespace {

const float RADIUS = 0.5f;

// 依 Stance 的順序：站、跪、坐
const std::array<collision::UprightCylinder, anatomy::STANCE_COUNT> COLLIDERS = {
    collision::UprightCylinder(Vec3(), RADIUS, 2.0f),
    collision::UprightCylinder(Vec3(), RADIUS, 1.4f),
    collision::UprightCylinder(Vec3(), RADIUS, 1.2f),
};

}  // namespace

Robot::Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain)
    : Entity(id, seed, position, std::move(brain)) {}

collision::UprightCylinder Robot::localCollider(anatomy::Stance stance) const {
    return COLLIDERS.at(static_cast<std::size_t>(stance));
}

bool Robot::supportsStance(anatomy::Stance stance) const {
    return stance != anatomy::Stance::Count;
}

}  // namespace robot::world::entity
