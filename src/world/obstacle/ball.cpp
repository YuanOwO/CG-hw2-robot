#include "world/obstacle/ball.hpp"

#include "geometry/mesh/mesh_builder.hpp"
#include "math/mat4.hpp"
#include "world/utils.hpp"

namespace robot::world::obstacle {

Ball::Ball(Seed seed, Vec3 position, float radius)
    : Obstacle(seed, position), _radius(radius), _collider(Vec3(0.0f, radius, 0.0f), radius) {
    geometry::MeshBuilder builder;

    builder.setColor(randomColor(seed, position.x, position.y, position.z));
    builder.transform(translation(Vec3(0.0f, radius, 0.0f)));

    builder.sphere(radius);

    setMesh(builder.build());
}

collision::Collider Ball::localCollider() const {
    return _collider;
}

}  // namespace robot::world::obstacle
