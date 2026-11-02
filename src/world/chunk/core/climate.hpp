#pragma once

#include "math/perlin.hpp"
#include "math/random.hpp"

namespace robot::world::chunk {

struct ClimateSample {
    float continent = 0.0f;    ///< 0.0 以下是海洋，越大越陸地
    float temperature = 0.5f;  ///< [0.0, 1.0]，越大越熱
    float humidity = 0.5f;     ///< [0.0, 1.0]，越大越潮濕
};

class ClimateSampler {
   public:
    explicit ClimateSampler(Seed seed);
    ~ClimateSampler() = default;

    float continent(const float worldX, const float worldZ) const;
    float temperature(const float worldX, const float worldZ) const;
    float humidity(const float worldX, const float worldZ) const;

    ClimateSample sample(const float worldX, const float worldZ) const;

   private:
    Perlin2D _warpXNoise;
    Perlin2D _warpZNoise;
    Perlin2D _continentNoise;
    Perlin2D _temperatureNoise;
    Perlin2D _humidityNoise;

    void warp(const float worldX, const float worldZ, float& outX, float& outZ) const;
};

}  // namespace robot::world::chunk
