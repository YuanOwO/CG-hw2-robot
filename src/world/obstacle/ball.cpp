#include "world/obstacle/ball.hpp"

namespace robot::world::obstacle {

Ball::Ball(Seed seed, Vec3 position, float radius)
    : Obstacle(seed, position), _radius(radius), _collider(Vec3(0.0f, radius, 0.0f), radius) {}

collision::Collider Ball::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
