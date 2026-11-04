#pragma once

#include "world/obstacle/obstacle.hpp"

namespace robot::view {

class ObstacleRenderer {
   public:
    void render(const world::obstacle::Obstacle& obstacle) const;
};

}  // namespace robot::view
