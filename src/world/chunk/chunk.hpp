#pragma once

#include <array>
#include <cstddef>
#include <functional>

#include "world/chunk/core/biome.hpp"
#include "world/chunk/core/surface.hpp"

namespace robot::world::chunk {

constexpr int CHUNK_SIZE = 16;                      // 邊長
constexpr int CHUNK_VERTEX_COUNT = CHUNK_SIZE + 1;  // 頂點數，包含邊界

// chunk 座標：第幾個 chunk，乘上 CHUNK_SIZE 即為該 chunk 角落的世界座標。
struct ChunkPos {
    int x = 0;
    int z = 0;

    static ChunkPos containing(float worldX, float worldZ);

    bool operator==(const ChunkPos& other) const { return x == other.x && z == other.z; }
    bool operator!=(const ChunkPos& other) const { return !(*this == other); }
};

// 兩個 32 位元座標合成一個 64 位元整數再雜湊，負座標也不會衝突
struct ChunkPosHash {
    std::size_t operator()(const ChunkPos& pos) const;
};

// 世界中一塊 CHUNK_SIZE × CHUNK_SIZE 的正方形區域。
class Chunk {
   public:
    using Heights = std::array<float, CHUNK_VERTEX_COUNT * CHUNK_VERTEX_COUNT>;
    using Biomes = std::array<Biome, CHUNK_SIZE * CHUNK_SIZE>;
    using Surfaces = std::array<Surface, CHUNK_SIZE * CHUNK_SIZE>;

    Chunk(ChunkPos pos, Heights heights, Heights waterLevels, Biomes biomes, Surfaces surfaces);

    ChunkPos pos() const { return _pos; }
    float heightAt(float worldX, float worldZ) const;
    float vertexHeight(int localX, int localZ) const;
    Biome biomeAt(int localX, int localZ) const;
    Surface surfaceAt(int localX, int localZ) const;

   private:
    ChunkPos _pos;
    Heights _heights;
    Heights _waterLevels;
    Biomes _biomes;
    Surfaces _surfaces;
};

}  // namespace robot::world::chunk
