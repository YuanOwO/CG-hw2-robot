#include "world/obstacle/pillar.hpp"

namespace robot::world::obstacle {

Pillar::Pillar(Seed seed, Vec3 position, float radius, float height)
    : Obstacle(seed, position), _radius(radius), _height(height), _collider(Vec3(0.0f, 0.0f, 0.0f), radius, height) {}

collision::Collider Pillar::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
