#pragma once

#include <memory>

#include "math/random.hpp"
#include "world/chunk/chunk.hpp"

namespace robot::world::chunk {

class ChunkGenerator {
   public:
    explicit ChunkGenerator(Seed seed);
    ~ChunkGenerator() = default;

    std::unique_ptr<Chunk> generate(ChunkPos pos) const;

   private:
    Seed _seed;
};

}  // namespace robot::world::chunk
