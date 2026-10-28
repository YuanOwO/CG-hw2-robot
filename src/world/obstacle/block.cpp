#include "world/obstacle/block.hpp"

namespace robot::world::obstacle {

Block::Block(Seed seed, Vec3 position, float yaw, Vec3 size)
    : Obstacle(seed, position, yaw), _size(size), _collider(Vec3(), size, yaw) {}

collision::Collider Block::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
