#include "world/obstacle/pillar.hpp"

#include "geometry/mesh/mesh_builder.hpp"
#include "math/mat4.hpp"
#include "world/utils.hpp"

namespace robot::world::obstacle {

Pillar::Pillar(Seed seed, Vec3 position, float radius, float height)
    : Obstacle(seed, position), _radius(radius), _height(height), _collider(Vec3(0.0f, 0.0f, 0.0f), radius, height) {
    geometry::MeshBuilder builder;

    builder.setColor(randomColor(seed, position.x, position.y, position.z));

    builder.cylinder(radius, height);

    setMesh(builder.build());
}

collision::Collider Pillar::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
