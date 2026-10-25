#pragma once

#include "common/color.hpp"
#include "world/chunk/chunk.hpp"

namespace robot::view {

class ChunkRenderer {
   public:
    void render(const world::chunk::Chunk& chunk) const;

   private:
    static const ColorRGBA GRASS_LIGHT;
    static const ColorRGBA GRASS_DARK;
};

}  // namespace robot::view
