#pragma once

#include <optional>

#include "world/collision/collider.hpp"
#include "world/collision/contact.hpp"

namespace robot::world::collision {

std::optional<Contact> detect(const UprightCylinder& player, const UprightBox& obstacle);
std::optional<Contact> detect(const UprightCylinder& player, const UprightCylinder& obstacle);
std::optional<Contact> detect(const UprightCylinder& player, const Sphere& obstacle);

std::optional<Contact> detect(const UprightCylinder& player, const Collider& obstacle);

}  // namespace robot::world::collision
