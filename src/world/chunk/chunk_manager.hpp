#pragma once

#include <memory>
#include <unordered_map>

#include "world/chunk/chunk.hpp"
#include "world/chunk/chunk_generator.hpp"

namespace robot::world::chunk {

class ChunkManager {
   public:
    explicit ChunkManager(std::uint64_t seed);
    ~ChunkManager() = default;

    void update(ChunkPos center);

    const Chunk* chunkAt(ChunkPos pos) const;  // 沒載入就回傳 nullptr

    template <typename F>
    void forEach(F&& visit) const {
        for (const auto& [pos, chunk] : _chunks) {
            visit(*chunk);
        }
    }

   private:
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>, ChunkPosHash> _chunks;

    ChunkGenerator _generator;
};

}  // namespace robot::world::chunk
