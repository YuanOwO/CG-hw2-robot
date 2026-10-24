#pragma once

#include "world/entity/entity.hpp"

namespace robot::view {

class EntityRenderer {
   public:
    void render(const world::entity::Entity& entity) const;
};

}  // namespace robot::view
