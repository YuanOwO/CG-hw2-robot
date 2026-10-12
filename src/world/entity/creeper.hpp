#pragma once

#include "world/entity/mob.hpp"

namespace robot::world::entity {

class Creeper : public Mob {
   public:
    Creeper(EntityId id, std::uint64_t seed, const Vec3& position, float yaw);
    ~Creeper() = default;

   protected:
    void aiStep(float dt, const World& world) override;
};

}  // namespace robot::world::entity
