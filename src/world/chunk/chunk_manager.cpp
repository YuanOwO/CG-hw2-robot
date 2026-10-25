#include "world/chunk/chunk_manager.hpp"

namespace robot::world::chunk {

namespace {

constexpr int CHUNK_LOAD_RADIUS = 5;

// 以 chunk 為單位的圓形範圍；比較平方，不必開根號。載入與卸載都用這個判斷，範圍才會一致
bool withinLoadRadius(ChunkPos pos, ChunkPos center) {
    const int dx = pos.x - center.x;
    const int dz = pos.z - center.z;
    return dx * dx + dz * dz < CHUNK_LOAD_RADIUS * CHUNK_LOAD_RADIUS;
}

}  // namespace

ChunkManager::ChunkManager(Seed seed) : _generator(seed) {}

const Chunk* ChunkManager::chunkAt(ChunkPos pos) const {
    auto it = _chunks.find(pos);
    if (it == _chunks.end()) {
        return nullptr;
    }
    return it->second.get();
}

void ChunkManager::update(ChunkPos center) {
    for (int dx = -CHUNK_LOAD_RADIUS; dx <= CHUNK_LOAD_RADIUS; dx++) {
        for (int dz = -CHUNK_LOAD_RADIUS; dz <= CHUNK_LOAD_RADIUS; dz++) {
            const ChunkPos pos{center.x + dx, center.z + dz};
            if (withinLoadRadius(pos, center) && _chunks.find(pos) == _chunks.end()) {
                _chunks.emplace(pos, _generator.generate(pos));
            }
        }
    }

    for (auto it = _chunks.begin(); it != _chunks.end();) {
        if (!withinLoadRadius(it->first, center)) {
            it = _chunks.erase(it);
        } else {
            it++;
        }
    }
}

float ChunkManager::heightAt(float worldX, float worldZ) const {
    const ChunkPos pos = ChunkPos::containing(worldX, worldZ);
    const Chunk* chunk = chunkAt(pos);
    if (chunk) {
        // 如果該 chunk 已載入，就直接用它的高度資料
        return chunk->heightAt(worldX, worldZ);
    }
    return _generator.heightAt(worldX, worldZ);
}

}  // namespace robot::world::chunk
