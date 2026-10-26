#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "math/utils.hpp"
#include "world/chunk/surface.hpp"

namespace robot::world::chunk {

namespace {

constexpr float WARP_NOISE_SCALE = 256.0f;
constexpr float WARP_NOISE_STRENGTH = 144.0f;

constexpr float CONTINENT_SCALE = 1024.0f;
constexpr float TEMPERATURE_SCALE = 384.0f;
constexpr float HUMIDITY_SCALE = 384.0f;

constexpr float CLIFF_FOOT = 0.003f;
constexpr float CLIFF_WIDTH = 0.009f;
constexpr float CLIFF_HEIGHT = 10.0f;

constexpr float ROUGHNESS_SCALE = 512.0f;
constexpr float MOUNTAIN_SCALE = 188.0f;
constexpr float MOUNTAIN_WARP_SCALE = 448.0f;
constexpr float MOUNTAIN_WARP_STRENGTH = 72.0f;
constexpr float MOUNTAIN_HEIGHT = 50.0f;
constexpr float HILL_SCALE = 64.0f;
constexpr float HILL_HEIGHT = 8.0f;

constexpr float DETAIL_SCALE = 48.0f;
constexpr float DETAIL_HEIGHT = 6.0f;

constexpr float SEA_LEVEL_BIAS = 0.3010299956639812f;

}  // namespace

ChunkGenerator::ChunkGenerator(Seed seed)
    : _seed(seed),
      _mainNoise(seed, fnv1a32("MAIN")),
      _warpXNoise(seed, fnv1a32("WARP_X")),
      _warpZNoise(seed, fnv1a32("WARP_Z")),
      _temperatureNoise(seed, fnv1a32("TEMPERATURE")),
      _humidityNoise(seed, fnv1a32("HUMIDITY")),
      _roughnessNoise(seed, fnv1a32("ROUGHNESS")),
      _mountainNoise(seed, fnv1a32("MOUNTAIN")),
      _hillNoise(seed, fnv1a32("HILL")),
      _detailNoise(seed, fnv1a32("DETAIL")) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    int originX = pos.x * CHUNK_SIZE;
    int originZ = pos.z * CHUNK_SIZE;

    Chunk::Heights heights;
    Chunk::Surfaces surfaces;
    using Array = std::array<float, CHUNK_VERTEX_COUNT * CHUNK_VERTEX_COUNT>;
    Array landness;
    Array temperature;
    Array humidity;

    auto indexOf = [&](int localX, int localZ) { return localZ * CHUNK_VERTEX_COUNT + localX; };

    for (int x = 0; x < CHUNK_VERTEX_COUNT; x++) {
        for (int z = 0; z < CHUNK_VERTEX_COUNT; z++) {
            float worldX = static_cast<float>(originX + x);
            float worldZ = static_cast<float>(originZ + z);
            ClimateSample climate = sampleClimate(worldX, worldZ);

            heights[indexOf(x, z)] = terrainHeight(worldX, worldZ, climate);
            landness[indexOf(x, z)] = climate.continent;
            temperature[indexOf(x, z)] = climate.temperature;
            humidity[indexOf(x, z)] = climate.humidity;
        }
    }

    auto cellAverage = [&](Array& arr, int x, int z) {
        float a = arr[indexOf(x, z)];
        float b = arr[indexOf(x, z + 1)];
        float c = arr[indexOf(x + 1, z + 1)];
        float d = arr[indexOf(x + 1, z)];
        return (a + b + c + d) / 4.0f;
    };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float h00 = heights[indexOf(x, z)];
            float h01 = heights[indexOf(x, z + 1)];
            float h11 = heights[indexOf(x + 1, z + 1)];
            float h10 = heights[indexOf(x + 1, z)];

            SurfaceInput input;
            input.x = static_cast<float>(originX + x) + 0.5f;
            input.z = static_cast<float>(originZ + z) + 0.5f;
            input.height = cellHeight(h00, h01, h11, h10);
            input.slope = cellSlope(h00, h01, h11, h10);
            input.landness = cellAverage(landness, x, z);
            input.temperature = cellAverage(temperature, x, z);
            input.humidity = cellAverage(humidity, x, z);

            surfaces[z * CHUNK_SIZE + x] = classifySurface(input);
        }
    }

    auto chunk = std::make_unique<Chunk>(pos, heights, surfaces);
    return chunk;
}

float ChunkGenerator::heightAt(float worldX, float worldZ) const {
    return terrainHeight(worldX, worldZ, sampleContinent(worldX, worldZ));
}

ChunkGenerator::ClimateSample ChunkGenerator::sampleContinent(float worldX, float worldZ) const {
    ClimateSample sample;
    float warpX, warpZ;
    warpNoise(worldX, worldZ, warpX, warpZ, WARP_NOISE_SCALE, WARP_NOISE_STRENGTH, 0.0f);

    sample.continent = _mainNoise.fbm(warpX / CONTINENT_SCALE, warpZ / CONTINENT_SCALE, 4) * 2.0f;

    // 大陸性的修正值
    sample.continent = sample.continent + SEA_LEVEL_BIAS;

    return sample;
}

ChunkGenerator::ClimateSample ChunkGenerator::sampleClimate(float worldX, float worldZ) const {
    ClimateSample sample = sampleContinent(worldX, worldZ);

    sample.temperature = _temperatureNoise.fbm(worldX / TEMPERATURE_SCALE, worldZ / TEMPERATURE_SCALE, 6) + 0.5f;
    sample.humidity = _humidityNoise.fbm(worldX / HUMIDITY_SCALE, worldZ / HUMIDITY_SCALE, 6) + 0.5f;

    return sample;
}

void ChunkGenerator::warpNoise(float worldX, float worldZ, float& outX, float& outZ, float scale, float strength,
                               float offset) const {
    float noiseX = _warpXNoise.fbm(worldX / scale + offset, worldZ / scale, 3);
    float noiseZ = _warpZNoise.fbm(worldX / scale + offset, worldZ / scale, 3);

    outX = worldX + noiseX * strength;
    outZ = worldZ + noiseZ * strength;
}

float ChunkGenerator::terrainHeight(float worldX, float worldZ, const ClimateSample& climate) const {
    // 大於 0 是陸地
    const float land = climate.continent;

    // 海底往下比陸地往上陡
    float base = land < 0.0f ? land * 25.0f : land * 10.0f;

    // 海平面附近的起伏會讓陸地反覆穿過 0，海岸碎成水窪；越接近海岸線起伏越小
    const float coastMask = smoothStep(0.0f, 0.35f, std::abs(land));
    float detail = _detailNoise.fbm(worldX / DETAIL_SCALE, worldZ / DETAIL_SCALE, 5);

    base += coastMask * DETAIL_HEIGHT * detail;

    // 海底地形已經結束
    if (land <= CLIFF_FOOT) {
        return base;
    }

    const float roughness = _roughnessNoise.fbm(worldX / ROUGHNESS_SCALE, worldZ / ROUGHNESS_SCALE, 2) + 0.5f;
    const float inlandMask = smoothStep(0.1f, 0.7f, land);

    // 海岸斷崖
    const float cliffMask = smoothStep(0.65f, 0.8f, roughness);
    const float cliffRise = smoothStep(CLIFF_FOOT, CLIFF_FOOT + CLIFF_WIDTH, land);
    base += cliffMask * CLIFF_HEIGHT * cliffRise * std::max(1.0f - inlandMask, 0.0f);  // 越接近海岸越陡峭

    // 丘陵
    const float hillMask = inlandMask * smoothStep(0.3f, 0.7f, roughness);
    if (hillMask > 0.0f) {
        float hill = _hillNoise.fbm(worldX / HILL_SCALE, worldZ / HILL_SCALE, 3) * 2.0f;

        base += hillMask * HILL_HEIGHT * std::max(hill * 0.5f + 0.5f, 0.0f);  // [0, 1]，丘陵不會太高
    }

    // 山脈
    const float mountainMask = inlandMask * smoothStep(0.6f, 0.8f, roughness);
    if (mountainMask > 0.0f) {
        float warpX, warpZ;
        warpNoise(worldX, worldZ, warpX, warpZ, MOUNTAIN_WARP_SCALE, MOUNTAIN_WARP_STRENGTH, 66.07f);
        const float mountain = _mountainNoise.fbm(warpX / MOUNTAIN_SCALE, warpZ / MOUNTAIN_SCALE, 2) * 2.0f;

        // (1 - |n|)^2 在零交叉處形成尖脊
        float ridge = 1.0f - std::abs(mountain);
        ridge = std::clamp(ridge, 0.0f, 1.0f);
        ridge *= ridge;

        const float detailScale = MOUNTAIN_SCALE / 4.0f;
        float mountainDetail = _mountainNoise.fbm(warpX / detailScale, warpZ / detailScale, 5);

        base += mountainMask * MOUNTAIN_HEIGHT * ridge * (1.0f + 0.5f * mountainDetail);
    }

    return base;
}

}  // namespace robot::world::chunk
