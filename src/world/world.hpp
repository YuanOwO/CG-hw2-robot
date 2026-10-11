#pragma once

#include <memory>
#include <unordered_map>

#include "world/chunk.hpp"

namespace robot::world {

const int WORLD_CHUNKS = 4;  // 每邊的 chunk 數
const float WORLD_SIZE = static_cast<float>(CHUNK_SIZE * WORLD_CHUNKS);

// 世界的狀態與規則，不涉及繪製與輸入。
// 範圍是 xz 平面上的 [0, WORLD_SIZE) × [0, WORLD_SIZE)，地面為 y = 0。
class World {
   public:
    World();

    void tick(float dt);

    bool contains(float x, float z) const;

    // 尚未生成的 chunk 回傳 nullptr。
    const Chunk* chunkAt(ChunkPos pos) const;

    // 走訪所有已生成的 chunk，順序不固定。
    template <typename F>
    void forEachChunk(F&& visit) const {
        for (const auto& entry : _chunks) {
            visit(*entry.second);
        }
    }

   private:
    // 只存已生成的 chunk，因此可以往任意方向延伸。
    // 值以 unique_ptr 保存：雜湊表擴充時會搬動元素，Chunk 本身的位址才不會跟著改變。
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>, ChunkPosHash> _chunks;
};

}  // namespace robot::world
