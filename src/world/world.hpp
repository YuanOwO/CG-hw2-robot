#pragma once

#include "math/vec3.hpp"
#include "world/chunk/chunk_manager.hpp"
#include "world/chunk/core/biome.hpp"
#include "world/entity/entity_manager.hpp"
#include "world/physics/simulator.hpp"

namespace robot::world {

// 世界的狀態與規則，不涉及繪製與輸入。
class World {
   public:
    explicit World(Seed seed);
    void tick(float dt);

    const chunk::Biome biomeAt(float worldX, float worldZ) const;

    Vec3 spawnPosition() const;

    void setFocus(Vec3 position);

    const chunk::ChunkManager& chunks() const { return _chunkManager; }
    entity::EntityManager& entities() { return _entityManager; }
    const entity::EntityManager& entities() const { return _entityManager; }
    const physics::Simulator& physics() const { return _simulator; }

   private:
    Seed _seed;
    chunk::ChunkManager _chunkManager;
    entity::EntityManager _entityManager;
    physics::Simulator _simulator;
};

}  // namespace robot::world
