#pragma once

#include "common/color.hpp"
#include "world/chunk/chunk.hpp"

namespace robot::view {

class ChunkRenderer {
   public:
    void render(const world::chunk::Chunk& chunk) const;

   private:
    ColorRGBA cellColor(const world::chunk::Chunk& chunk, int localX, int localZ) const;
};

}  // namespace robot::view
