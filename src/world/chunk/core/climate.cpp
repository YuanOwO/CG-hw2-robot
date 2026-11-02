#include "world/chunk/core/climate.hpp"

#include <algorithm>

#include "math/utils.hpp"

namespace robot::world::chunk {

ClimateSampler::ClimateSampler(Seed seed)
    : _warpXNoise(seed, fnv1a32("WARP_X")),
      _warpZNoise(seed, fnv1a32("WARP_Z")),
      _continentNoise(seed, fnv1a32("CONTINENT")),
      _temperatureNoise(seed, fnv1a32("TEMPERATURE")),
      _humidityNoise(seed, fnv1a32("HUMIDITY")) {}

float ClimateSampler::continent(const float worldX, const float worldZ) const {
    static constexpr float CONTINENT_SCALE = 1024.0f;

    float warpX, warpZ;
    warp(worldX, worldZ, warpX, warpZ);
    auto noise = _continentNoise.fbm(warpX / CONTINENT_SCALE, warpZ / CONTINENT_SCALE, 4);
    return noise * 1.46f;  // 98% 分佈在 [-1.0, 1.0]
}

float ClimateSampler::temperature(const float worldX, const float worldZ) const {
    static constexpr float TEMPERATURE_SCALE = 384.0f;

    auto noise = _temperatureNoise.fbm(worldX / TEMPERATURE_SCALE, worldZ / TEMPERATURE_SCALE, 6);
    noise = std::clamp(noise * 1.46f, -1.0f, 1.0f);  // 98% 分佈在 [-1.0, 1.0]，其餘 2% 會被截斷
    return noise * 0.5f + 0.5f;
}

float ClimateSampler::humidity(const float worldX, const float worldZ) const {
    static constexpr float HUMIDITY_SCALE = 384.0f;

    auto noise = _humidityNoise.fbm(worldX / HUMIDITY_SCALE, worldZ / HUMIDITY_SCALE, 6);
    noise = std::clamp(noise * 1.46f, -1.0f, 1.0f);  // 98% 分佈在 [-1.0, 1.0]，其餘 2% 會被截斷
    return noise * 0.5f + 0.5f;
}

ClimateSample ClimateSampler::sample(const float worldX, const float worldZ) const {
    ClimateSample result;
    result.continent = continent(worldX, worldZ);
    result.temperature = temperature(worldX, worldZ);
    result.humidity = humidity(worldX, worldZ);
    return result;
}

void ClimateSampler::warp(const float worldX, const float worldZ, float& outX, float& outZ) const {
    static constexpr float WARP_NOISE_SCALE = 256.0f;
    static constexpr float WARP_NOISE_STRENGTH = 94.3f;

    float noiseX = _warpXNoise.fbm(worldX / WARP_NOISE_SCALE, worldZ / WARP_NOISE_SCALE, 3);
    float noiseZ = _warpZNoise.fbm(worldX / WARP_NOISE_SCALE, worldZ / WARP_NOISE_SCALE, 3);

    outX = worldX + noiseX * WARP_NOISE_STRENGTH;
    outZ = worldZ + noiseZ * WARP_NOISE_STRENGTH;
}

}  // namespace robot::world::chunk
