#include "world/entity/entity_manager.hpp"

namespace robot::world::entity {

EntityManager::EntityManager(Seed seed) : _seed(seed) {}

void EntityManager::tick(float dt, const World& world) {
    for (const auto& entity : _entities) {
        entity->tick(dt, world);
    }
}

}  // namespace robot::world::entity
