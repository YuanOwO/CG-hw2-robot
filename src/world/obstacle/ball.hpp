#pragma once

#include "math/vec3.hpp"
#include "world/collision/collision.hpp"
#include "world/obstacle/obstacle.hpp"

namespace robot::world::obstacle {

class Ball : public Obstacle {
   public:
    Ball(Seed seed, Vec3 position, float radius = 1.0f);

   protected:
    collision::Collider localCollider() const override;

   private:
    float _radius;
    collision::Sphere _collider;
};

}  // namespace robot::world::obstacle
