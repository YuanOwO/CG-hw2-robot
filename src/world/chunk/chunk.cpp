#include "world/chunk/chunk.hpp"

#include <cassert>

namespace robot::world::chunk {

float Chunk::heightAt(int localX, int localZ) const {
    assert(localX >= 0 && localX < CHUNK_VERTEX_COUNT && localZ >= 0 && localZ < CHUNK_VERTEX_COUNT);
    return heights[static_cast<std::size_t>(localZ * CHUNK_VERTEX_COUNT + localX)];
}

}  // namespace robot::world::chunk
