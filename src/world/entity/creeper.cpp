#include "world/entity/creeper.hpp"

namespace robot::world::entity {

Creeper::Creeper(EntityId id, std::uint64_t seed, const Vec3& position, float yaw)
    : Mob(id, seed, position, yaw, 1.0f, 90.0f) {}

void Creeper::aiStep(float dt, const World& world) {
    // 先隨便走，之後再加上 AI
    setMoveIntent(yaw() + 30.0f * dt, 1.0f);
}

}  // namespace robot::world::entity
