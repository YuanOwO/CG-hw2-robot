#pragma once

#include <cstdint>
#include <memory>

#include "world/chunk/chunk.hpp"

namespace robot::world::chunk {

class ChunkGenerator {
   public:
    explicit ChunkGenerator(std::uint64_t seed);
    ~ChunkGenerator() = default;

    std::unique_ptr<Chunk> generate(ChunkPos pos) const;

   private:
    std::uint64_t _seed;
};

}  // namespace robot::world::chunk
