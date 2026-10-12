#pragma once

#include <cstdint>

#include "world/chunk/chunk_manager.hpp"
#include "world/entity/entity_manager.hpp"

namespace robot::world {

// 世界的狀態與規則，不涉及繪製與輸入。
class World {
   public:
    explicit World(std::uint64_t seed);
    void tick(float dt);

    bool blockedAt(float worldX, float worldZ, float radius) const { return false; }  // TODO

    const entity::EntityManager& entities() const { return _entityManager; }

   private:
    std::uint64_t _seed;
    chunk::ChunkManager _chunkManager;
    entity::EntityManager _entityManager;
};

}  // namespace robot::world
