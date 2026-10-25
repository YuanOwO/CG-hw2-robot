#pragma once

#include "world/chunk/chunk.hpp"

namespace robot::view {

class ChunkRenderer {
   public:
    void render(const world::chunk::Chunk& chunk) const;
};

}  // namespace robot::view
