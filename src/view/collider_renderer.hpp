#pragma once

#include "world/collision/collision.hpp"

namespace robot::view {

void drawCollider(const world::collision::UprightBox& collider);
void drawCollider(const world::collision::UprightCylinder& collider);
void drawCollider(const world::collision::Sphere& collider);

void drawCollider(const world::collision::Collider& collider);

}  // namespace robot::view
