#pragma once

#include "math/vec3.hpp"
#include "world/collision/collider.hpp"
#include "world/obstacle/obstacle.hpp"

namespace robot::world::obstacle {

class Block : public Obstacle {
   public:
    Block(Seed seed, Vec3 position, float yaw = 0.0f, Vec3 size = Vec3(1.0f, 1.0f, 1.0f));

   protected:
    collision::Collider localCollider() const override;

   private:
    Vec3 _size;
    collision::UprightBox _collider;
};

}  // namespace robot::world::obstacle
