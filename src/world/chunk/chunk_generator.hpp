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

    float heightAt(float worldX, float worldZ) const;

   private:
    Seed _seed;
    Perlin2D _mainNoise;
    Perlin2D _temperatureNoise;
    Perlin2D _humidityNoise;
    Perlin2D _roughnessNoise;
    Perlin2D _mountainNoise;
    Perlin2D _detailNoise;

    struct ClimateSample {
        float continent;    /// 0.0 以下是海洋，越大越陸地
        float temperature;  /// [0.0, 1.0]，越大越熱
        float humidity;     /// [0.0, 1.0]，越大越潮濕
        float roughness;    /// [0.0, 1.0]，越大越粗糙
    };

    ClimateSample sampleClimate(float worldX, float worldZ) const;

    float terrainHeight(float worldX, float worldZ, const ClimateSample& climate) const;
};

}  // namespace robot::world::chunk
