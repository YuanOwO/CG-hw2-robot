#include "world/chunk/chunk.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <utility>

#include "math/utils.hpp"

namespace robot::world::chunk {

ChunkPos ChunkPos::containing(float worldX, float worldZ) {
    int cx = static_cast<int>(floorDiv(worldX, CHUNK_SIZE));
    int cz = static_cast<int>(floorDiv(worldZ, CHUNK_SIZE));
    return {cx, cz};
}

std::size_t ChunkPosHash::operator()(const ChunkPos& pos) const {
    std::uint64_t ux = static_cast<std::uint32_t>(pos.x);
    std::uint64_t uz = static_cast<std::uint32_t>(pos.z);
    const std::uint64_t key = ux << 32 | uz;  // 兩個 32 位元座標合成一個 64 位元整數
    return std::hash<std::uint64_t>{}(key);
}

Chunk::Chunk(ChunkPos pos, Heights heights, Heights waterLevels, Biomes biomes, Surfaces surfaces)
    : _pos(pos),
      _heights(std::move(heights)),
      _waterLevels(std::move(waterLevels)),
      _biomes(std::move(biomes)),
      _surfaces(std::move(surfaces)) {}

float Chunk::heightAt(float worldX, float worldZ) const {
    float localX = worldX - _pos.x * CHUNK_SIZE;
    float localZ = worldZ - _pos.z * CHUNK_SIZE;

    // 計算所在的格子
    int cellX = std::clamp(static_cast<int>(std::floor(localX)), 0, CHUNK_SIZE - 1);
    int cellZ = std::clamp(static_cast<int>(std::floor(localZ)), 0, CHUNK_SIZE - 1);

    // 計算格子內的相對位置
    float fracX = localX - cellX;  // [0, 1]
    float fracZ = localZ - cellZ;  // [0, 1]

    // 取得四個頂點的高度
    float h00 = vertexHeight(cellX, cellZ);
    float h01 = vertexHeight(cellX, cellZ + 1);
    float h11 = vertexHeight(cellX + 1, cellZ + 1);
    float h10 = vertexHeight(cellX + 1, cellZ);

    if (fracX <= fracZ) {
        // 在 h00, h01, h11 的平面上
        return h00 + (h01 - h00) * fracZ + (h11 - h01) * fracX;
    } else {
        // 在 h00, h11, h10 的平面上
        return h00 + (h10 - h00) * fracX + (h11 - h10) * fracZ;
    }
}

float Chunk::vertexHeight(int localX, int localZ) const {
    assert(localX >= 0 && localX < CHUNK_VERTEX_COUNT && localZ >= 0 && localZ < CHUNK_VERTEX_COUNT);
    std::size_t index = static_cast<std::size_t>(localZ * CHUNK_VERTEX_COUNT + localX);
    return _heights[index];
}

Biome Chunk::biomeAt(int localX, int localZ) const {
    assert(localX >= 0 && localX < CHUNK_SIZE && localZ >= 0 && localZ < CHUNK_SIZE);
    std::size_t index = static_cast<std::size_t>(localZ * CHUNK_SIZE + localX);
    return _biomes[index];
}

SurfaceType Chunk::surfaceAt(int localX, int localZ) const {
    assert(localX >= 0 && localX < CHUNK_SIZE && localZ >= 0 && localZ < CHUNK_SIZE);
    std::size_t index = static_cast<std::size_t>(localZ * CHUNK_SIZE + localX);
    return _surfaces[index];
}

}  // namespace robot::world::chunk
