#pragma once

#include <memory>

#include "math/perlin.hpp"
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
    Perlin2D _mainNoise;
    Perlin2D _temperatureNoise;
    Perlin2D _humidityNoise;
    Perlin2D _roughnessNoise;
    Perlin2D _mountainNoise;
    Perlin2D _detailNoise;

    struct ClimateSample {
        float continent;
        float temperature;
        float humidity;
        float roughness;
    };

    ClimateSample sampleClimate(float worldX, float worldZ) const;

    float terrainHeight(float worldX, float worldZ) const;
};

}  // namespace robot::world::chunk
