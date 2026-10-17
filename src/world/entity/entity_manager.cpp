#include "world/entity/entity_manager.hpp"

namespace robot::world::entity {

namespace {

constexpr std::uint32_t ENTITY_SEED_STREAM = 0x454e5453u;  // "ENTS"

}  // namespace

EntityManager::EntityManager(std::uint64_t seed) : _random(seed) {}

void EntityManager::tick(float dt, const World& world) {
    for (const auto& entity : _entities) {
        entity->tick(dt, world);
    }
}

// 以 id 為序號：生成順序相同，seed 就相同
std::uint64_t EntityManager::seedFor(EntityId id) const {
    return _random.valueAt(ENTITY_SEED_STREAM, id);
}

}  // namespace robot::world::entity
