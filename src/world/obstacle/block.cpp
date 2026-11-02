#include "world/obstacle/block.hpp"

#include "geometry/mesh/mesh_builder.hpp"
#include "math/mat4.hpp"
#include "world/utils.hpp"

namespace robot::world::obstacle {

Block::Block(Seed seed, Vec3 position, float yaw, Vec3 size)
    : Obstacle(seed, position, yaw), _size(size), _collider(Vec3(), size, yaw) {
    geometry::MeshBuilder builder;

    builder.setColor(randomColor(seed, position.x, position.y, position.z));
    builder.transform(rotation(yaw, Vec3(0.0f, 1.0f, 0.0f)));

    auto min = -Vec3(size.x / 2.0f, 0.0f, size.z / 2.0f);
    auto max = Vec3(size.x / 2.0f, size.y, size.z / 2.0f);
    builder.cuboid(min, max);

    setMesh(builder.build());
}

collision::Collider Block::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
