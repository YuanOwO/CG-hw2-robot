#pragma once

#include <cstdint>

#include "world/chunk/chunk_manager.hpp"
#include "world/entity/entity_manager.hpp"
#include "world/entity/player.hpp"

namespace robot::world {

// 世界的狀態與規則，不涉及繪製與輸入。
class World {
   public:
    explicit World(std::uint64_t seed);
    void tick(float dt);

    bool blockedAt(float worldX, float worldZ, float radius) const { return false; }  // TODO

    const chunk::ChunkManager& chunks() const { return _chunkManager; }
    const entity::EntityManager& entities() const { return _entityManager; }

    // 外面只能對玩家下指令，不能直接改它的狀態，移動因此一定經過 LivingEntity 的規則
    void setPlayerCommand(const entity::PlayerCommand& command) { _player->setCommand(command); }
    const entity::Player& player() const { return *_player; }

   private:
    std::uint64_t _seed;
    chunk::ChunkManager _chunkManager;
    entity::EntityManager _entityManager;
    entity::Player* _player;  // 由 _entityManager 持有；實體以 unique_ptr 保存，位址不會變

    void loadChunksAroundPlayer();
};

}  // namespace robot::world
