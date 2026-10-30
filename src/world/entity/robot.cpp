#include "world/entity/robot.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace robot::world::entity {

namespace {

constexpr float ROBOT_MASS = 70.0f;   // kg
constexpr float ROBOT_HEIGHT = 2.0f;  // m
constexpr float ROBOT_RADIUS = 0.5f;  // m

constexpr float ROBOT_JUMP_HEIGHT = 0.5f;  // m
constexpr float ROBOT_SNEAK_SPEED = 0.6f;  // m/s
constexpr float ROBOT_WALK_SPEED = 1.2f;   // m/s
constexpr float ROBOT_RUN_SPEED = 10.0f;   // m/s

// 依 Stance 的順序：站、跪、坐
const std::array<collision::UprightCylinder, anatomy::STANCE_COUNT> COLLIDERS = {
    collision::UprightCylinder(Vec3(), ROBOT_RADIUS, ROBOT_HEIGHT),
    collision::UprightCylinder(Vec3(), ROBOT_RADIUS, ROBOT_HEIGHT * 0.75f),
    collision::UprightCylinder(Vec3(), ROBOT_RADIUS, ROBOT_HEIGHT * 0.5f),
};

}  // namespace

Robot::Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain)
    : Entity(id, seed, ROBOT_MASS, position, std::move(brain)) {}

collision::UprightCylinder Robot::localCollider(anatomy::Stance stance) const {
    return COLLIDERS.at(static_cast<std::size_t>(stance));
}

float Robot::jumpHeight() const {
    return ROBOT_JUMP_HEIGHT;
}

float Robot::sneakSpeed() const {
    return ROBOT_SNEAK_SPEED;
}

float Robot::walkSpeed() const {
    return ROBOT_WALK_SPEED;
}

float Robot::runSpeed() const {
    return ROBOT_RUN_SPEED;
}

bool Robot::supportsStance(anatomy::Stance stance) const {
    return stance != anatomy::Stance::Count;
}

}  // namespace robot::world::entity
