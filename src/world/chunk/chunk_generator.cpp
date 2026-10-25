#include "world/chunk/chunk_generator.hpp"

namespace robot::world::chunk {

ChunkGenerator::ChunkGenerator(Seed seed) : _seed(seed), _perlin(seed) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    int originX = pos.x * CHUNK_SIZE;
    int originZ = pos.z * CHUNK_SIZE;

    Chunk::Heights heights;

    for (int x = 0; x < CHUNK_VERTEX_COUNT; x++) {
        for (int z = 0; z < CHUNK_VERTEX_COUNT; z++) {
            float worldX = static_cast<float>(originX + x);
            float worldZ = static_cast<float>(originZ + z);

            // 以 Perlin 雜訊生成高度，並將輸出範圍 [-1, 1] 映射到 [0, 1]
            float height = _perlin.noise(worldX * 0.1f, worldZ * 0.1f) * 0.5f + 0.5f;
            heights[z * CHUNK_VERTEX_COUNT + x] = height;
        }
    }

    auto chunk = std::make_unique<Chunk>(pos, heights);
    return chunk;
}

}  // namespace robot::world::chunk
