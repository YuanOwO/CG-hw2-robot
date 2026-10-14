#pragma once

#include "world/entity/mob.hpp"

namespace robot::world::entity {

class Chicken : public Mob {
   public:
    Chicken(EntityId id, std::uint64_t seed, const Vec3& position, float yaw);

    static const anatomy::Rig& speciesRig();

   protected:
    void think(float dt, const World& world) override;
};

}  // namespace robot::world::entity
