#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "world/chunk/chunk.hpp"
#include "world/chunk/chunk_generator.hpp"
#include "world/chunk/core/biome.hpp"
#include "world/obstacle/obstacle.hpp"

namespace robot::world::chunk {

class ChunkManager {
   public:
    using ChunkHandler = std::function<void(const Chunk&)>;
    using ObstacleHandler = std::function<void(const obstacle::Obstacle&)>;

    explicit ChunkManager(Seed seed);
    ~ChunkManager() = default;

    void update(ChunkPos center);

    const Chunk* chunkAt(ChunkPos pos) const;  // 沒載入就回傳 nullptr

    float heightAt(float worldX, float worldZ) const;
    const Biome biomeAt(float worldX, float worldZ) const;

    void forEach(ChunkHandler&& visit) const {
        for (const auto& [pos, chunk] : _chunks) {
            visit(*chunk);
        }
    }

    void forEachObstacleNear(float worldX, float worldZ, ObstacleHandler&& visit) const {
        ChunkPos center = ChunkPos::containing(worldX, worldZ);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                const Chunk* chunk = chunkAt(ChunkPos{center.x + dx, center.z + dz});

                if (chunk == nullptr) {
                    continue;
                }

                for (const auto& obstacle : chunk->obstacles()) {
                    visit(*obstacle);
                }
            }
        }
    }

   private:
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>, ChunkPosHash> _chunks;

    ChunkGenerator _generator;
};

}  // namespace robot::world::chunk
