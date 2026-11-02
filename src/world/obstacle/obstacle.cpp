#include "world/obstacle/obstacle.hpp"

#include <utility>

namespace robot::world::obstacle {

Obstacle::Obstacle(Seed seed, Vec3 position, float yaw) : _seed(seed), _random(seed), _position(position), _yaw(yaw) {}

collision::Collider Obstacle::collider() const {
    return translated(localCollider(), _position);
}

void Obstacle::setMesh(geometry::Mesh&& mesh) {
    _mesh = std::move(mesh);
}

}  // namespace robot::world::obstacle
