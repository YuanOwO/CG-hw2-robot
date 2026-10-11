#include "world/world.hpp"

namespace robot::world {

World::World() {
    for (int z = 0; z < WORLD_CHUNKS; z++) {
        for (int x = 0; x < WORLD_CHUNKS; x++) {
            auto chunk = std::make_unique<Chunk>();
            chunk->pos = {x, z};
            _chunks.emplace(chunk->pos, std::move(chunk));
        }
    }
}

void World::tick(float dt) {}

bool World::contains(float x, float z) const {
    return x >= 0.0f && x < WORLD_SIZE && z >= 0.0f && z < WORLD_SIZE;
}

const Chunk* World::chunkAt(ChunkPos pos) const {
    const auto found = _chunks.find(pos);
    return found == _chunks.end() ? nullptr : found->second.get();
}

}  // namespace robot::world
