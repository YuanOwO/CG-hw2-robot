#include "world/world.hpp"

#include <cmath>

namespace robot::world {

namespace {

const float ROTATION_SPEED = 45.0f;  // 度 / 秒

}  // namespace

void World::tick(float dt) {
    _floorAngle = std::fmod(_floorAngle + ROTATION_SPEED * dt, 360.0f);
}

}  // namespace robot::world
