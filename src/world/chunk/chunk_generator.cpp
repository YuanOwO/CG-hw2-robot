#include "world/chunk/chunk_generator.hpp"

namespace robot::world::chunk {

ChunkGenerator::ChunkGenerator(std::uint64_t seed) : _seed(seed) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    auto chunk = std::make_unique<Chunk>();
    chunk->pos = pos;
    return chunk;
}

}  // namespace robot::world::chunk
