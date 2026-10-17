#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace robot::world::chunk {

const int CHUNK_SIZE = 16;                      // 邊長
const int CHUNK_VERTEX_COUNT = CHUNK_SIZE + 1;  // 頂點數，包含邊界

// chunk 座標：第幾個 chunk，乘上 CHUNK_SIZE 即為該 chunk 角落的世界座標。
struct ChunkPos {
    int x = 0;
    int z = 0;

    // 世界座標所在的 chunk。用 floor 而不是整數除法：整數除法向 0 取整，會把 -0.5 算進第 0 個 chunk。
    static ChunkPos containing(float worldX, float worldZ) {
        return {static_cast<int>(std::floor(worldX / CHUNK_SIZE)), static_cast<int>(std::floor(worldZ / CHUNK_SIZE))};
    }

    bool operator==(const ChunkPos& other) const { return x == other.x && z == other.z; }
    bool operator!=(const ChunkPos& other) const { return !(*this == other); }
};

// 兩個 32 位元座標合成一個 64 位元整數再雜湊，負座標也不會衝突
struct ChunkPosHash {
    std::size_t operator()(const ChunkPos& pos) const {
        const std::uint64_t key =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(pos.x)) << 32) | static_cast<std::uint32_t>(pos.z);
        return std::hash<std::uint64_t>{}(key);
    }
};

// 世界中一塊 CHUNK_SIZE × CHUNK_SIZE 的正方形區域。
struct Chunk {
    ChunkPos pos;
    std::array<float, CHUNK_VERTEX_COUNT * CHUNK_VERTEX_COUNT> heights{};
    float heightAt(int localX, int localZ) const;
};

}  // namespace robot::world::chunk
