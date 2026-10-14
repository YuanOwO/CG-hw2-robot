#pragma once

#include "world/entity/mob.hpp"

namespace robot::world::entity {

class Pig : public Mob {
   public:
    Pig(EntityId id, std::uint64_t seed, const Vec3& position, float yaw);

    static const anatomy::Rig& speciesRig();

   protected:
    void think(float dt, const World& world) override;
};

}  // namespace robot::world::entity
