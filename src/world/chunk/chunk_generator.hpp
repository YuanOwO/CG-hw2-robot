#pragma once

#include <memory>

#include "math/random.hpp"
#include "world/chunk/chunk.hpp"
#include "world/chunk/core/biome.hpp"
#include "world/chunk/core/climate.hpp"
#include "world/chunk/core/surface.hpp"
#include "world/chunk/core/terrain.hpp"

namespace robot::world::chunk {

class ChunkGenerator {
   public:
    explicit ChunkGenerator(Seed seed);
    ~ChunkGenerator() = default;

    std::unique_ptr<Chunk> generate(ChunkPos pos) const;

    float heightAt(float worldX, float worldZ) const;

   private:
    Random _random;

    BiomeClassifier _biomeClassifier;
    ClimateSampler _climateSampler;
    SurfaceResolver _surfaceResolver;
    TerrainSampler _terrainSampler;
};

}  // namespace robot::world::chunk
