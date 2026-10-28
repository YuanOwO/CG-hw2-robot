#pragma once

#include "world/chunk/chunk_manager.hpp"
#include "world/chunk/core/biome.hpp"
#include "world/collision/collider.hpp"
#include "world/entity/brain/player_brain.hpp"
#include "world/entity/entity_manager.hpp"
#include "world/entity/robot.hpp"
#include "world/types.hpp"

namespace robot::world {

// 世界的狀態與規則，不涉及繪製與輸入。
class World {
   public:
    explicit World(Seed seed);
    void tick(float dt);

    const chunk::Biome biomeAt(float worldX, float worldZ) const;

    Vec3 spawnPosition() const;

    void setFocus(Vec3 position);

    bool blockedAt(float worldX, float worldZ, float radius) const { return false; }  // TODO

    const chunk::ChunkManager& chunks() const { return _chunkManager; }
    entity::EntityManager& entities() { return _entityManager; }
    const entity::EntityManager& entities() const { return _entityManager; }

    MotionResult resolveMotion(const collision::UprightCylinder& collider, const Motion& motion,
                               bool wasOnGround) const;

   private:
    Seed _seed;
    chunk::ChunkManager _chunkManager;
    entity::EntityManager _entityManager;
};

}  // namespace robot::world
