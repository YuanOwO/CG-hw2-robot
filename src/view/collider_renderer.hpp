#pragma once

#include "world/collision/collision.hpp"

namespace robot::view {

void drawCollider(const world::collision::Box& collider);
void drawCollider(const world::collision::UprightCylinder& collider);

}  // namespace robot::view
