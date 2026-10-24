#include "world/chunk/chunk_generator.hpp"

namespace robot::world::chunk {

ChunkGenerator::ChunkGenerator(Seed seed) : _seed(seed) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    auto chunk = std::make_unique<Chunk>(pos, Chunk::Heights{});
    return chunk;
}

}  // namespace robot::world::chunk
