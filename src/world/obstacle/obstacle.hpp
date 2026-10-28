#pragma once

#include "math/random.hpp"
#include "math/vec3.hpp"
#include "world/collision/collider.hpp"

namespace robot::world::obstacle {

class Obstacle {
   public:
    virtual ~Obstacle() = default;

    Seed seed() const { return _seed; }
    const Random& random() const { return _random; }

    Vec3 position() const { return _position; }
    float yaw() const { return _yaw; }

    collision::Collider collider() const;

   protected:
    Obstacle(Seed seed, Vec3 position, float yaw = 0.0f);

    virtual collision::Collider localCollider() const = 0;

   private:
    Seed _seed;
    Random _random;

    Vec3 _position;
    float _yaw = 0.0f;
};

}  // namespace robot::world::obstacle
