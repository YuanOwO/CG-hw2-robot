#pragma once

#include "world/chunk/core/biome.hpp"

namespace robot::world::chunk {

enum class Surface {
    Grass,
    Stone,
    Sand,
    Snow,
    Water,
    Ice,

    Count,
};

struct SurfaceInput {
    Biome biome;
    float height;
    float waterLevel;
    float slope;
};

class SurfaceResolver {
   public:
    Surface resolve(const SurfaceInput& input) const;

   private:
    bool isFrozen(const Biome& biome) const;

    Surface defaultSurface(const Biome& biome) const;
};

}  // namespace robot::world::chunk
