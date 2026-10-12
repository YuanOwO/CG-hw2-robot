#include "world/chunk/chunk_manager.hpp"

namespace robot::world::chunk {

ChunkManager::ChunkManager(std::uint64_t seed) : _generator(seed) {}

void ChunkManager::update(ChunkPos center) {}

}  // namespace robot::world::chunk
