#pragma once

#include <memory>

#include "world/entity/entity.hpp"
#include "world/types.hpp"

namespace robot::world::entity {

class Robot final : public Entity {
   public:
    Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain);

   protected:
    float jumpHeight() const override;
    float sneakSpeed() const override;
    float walkSpeed() const override;
    float runSpeed() const override;

    collision::UprightCylinder localCollider(anatomy::Stance stance) const override;
    bool supportsStance(anatomy::Stance stance) const override;
};

}  // namespace robot::world::entity
