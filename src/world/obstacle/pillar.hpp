#pragma once

#include "math/vec3.hpp"
#include "world/collision/collision.hpp"
#include "world/obstacle/obstacle.hpp"

namespace robot::world::obstacle {

class Pillar : public Obstacle {
   public:
    Pillar(Seed seed, Vec3 position, float radius = 1.0f, float height = 2.0f);

   protected:
    collision::Collider localCollider() const override;

   private:
    float _radius;
    float _height;
    collision::UprightCylinder _collider;
};

}  // namespace robot::world::obstacle
