#pragma once

#include <memory>

#include "world/entity/entity.hpp"
#include "world/types.hpp"

namespace robot::world::entity {

class Robot final : public Entity {
   public:
    Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain);

   protected:
    float mass() const override { return 70.0f; }
    float jumpHeight() const override { return 0.5f; }
    float walkSpeed() const override { return 1.2f; }
    float runSpeed() const override { return 10.0f; }

    const collision::UprightCylinder& localCollider(anatomy::Stance stance) const override;
    bool supportsStance(anatomy::Stance stance) const override;
};

}  // namespace robot::world::entity
