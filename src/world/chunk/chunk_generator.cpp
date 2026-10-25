#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "math/utils.hpp"
#include "world/chunk/surface.hpp"

namespace robot::world::chunk {

namespace {

constexpr Stream TERRAIN_MAIN_NOISE_STREAM = fnv1a32("TERRAIN_MAIN_NOISE");
constexpr Stream TERRAIN_TEMPERATURE_NOISE_STREAM = fnv1a32("TERRAIN_TEMPERATURE_NOISE");
constexpr Stream TERRAIN_HUMIDITY_NOISE_STREAM = fnv1a32("TERRAIN_HUMIDITY_NOISE");
constexpr Stream TERRAIN_ROUGHNESS_NOISE_STREAM = fnv1a32("TERRAIN_ROUGHNESS_NOISE");
constexpr Stream TERRAIN_MOUNTAIN_NOISE_STREAM = fnv1a32("TERRAIN_MOUNTAIN_NOISE");
constexpr Stream TERRAIN_DETAIL_NOISE_STREAM = fnv1a32("TERRAIN_DETAIL_NOISE");

constexpr float CONTINENT_SCALE = 512.0f;
constexpr float TEMPERATURE_SCALE = 384.0f;
constexpr float HUMIDITY_SCALE = 128.0f;
constexpr float ROUGHNESS_SCALE = 512.0f;
constexpr float MOUNTAIN_SCALE = 128.0f;
constexpr float DETAIL_SCALE = 32.0f;

constexpr float SEA_LEVEL_BIAS = 0.3010299956639812f;

}  // namespace

ChunkGenerator::ChunkGenerator(Seed seed)
    : _seed(seed),
      _mainNoise(seed, TERRAIN_MAIN_NOISE_STREAM),
      _temperatureNoise(seed, TERRAIN_TEMPERATURE_NOISE_STREAM),
      _humidityNoise(seed, TERRAIN_HUMIDITY_NOISE_STREAM),
      _roughnessNoise(seed, TERRAIN_ROUGHNESS_NOISE_STREAM),
      _mountainNoise(seed, TERRAIN_MOUNTAIN_NOISE_STREAM),
      _detailNoise(seed, TERRAIN_DETAIL_NOISE_STREAM) {}

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
    return terrainHeight(worldX, worldZ, sampleClimate(worldX, worldZ));
}

ChunkGenerator::ClimateSample ChunkGenerator::sampleClimate(float worldX, float worldZ) const {
    ClimateSample sample;

    sample.continent = _mainNoise.fbm(worldX / CONTINENT_SCALE, worldZ / CONTINENT_SCALE, 4) * 2.0f;
    sample.temperature = _temperatureNoise.fbm(worldX / TEMPERATURE_SCALE, worldZ / TEMPERATURE_SCALE, 6) + 0.5f;
    sample.humidity = _humidityNoise.fbm(worldX / HUMIDITY_SCALE, worldZ / HUMIDITY_SCALE, 6) + 0.5f;
    sample.roughness = _roughnessNoise.fbm(worldX / ROUGHNESS_SCALE, worldZ / ROUGHNESS_SCALE, 2) + 0.5f;

    // 大陸性的修正值
    sample.continent = sample.continent + SEA_LEVEL_BIAS;

    return sample;
}

float ChunkGenerator::terrainHeight(float worldX, float worldZ, const ClimateSample& climate) const {
    // 大於 0 是陸地
    float land = climate.continent;

    // 海底往下比陸地往上陡，海岸附近才會是平緩的平原
    float base = land < 0.0f ? land * 25.0f : land * 10.0f;

    // 同時位於內陸且崎嶇才長山：沿海保持平原，內陸不崎嶇的地方也是平地
    float inlandMask = smoothStep(0.2f, 0.7f, land);
    float roughnessMask = smoothStep(0.4f, 0.78f, climate.roughness);
    float mountainMask = inlandMask * roughnessMask;

    float mountain = _mountainNoise.fbm(worldX / MOUNTAIN_SCALE, worldZ / MOUNTAIN_SCALE, 4) * 2.0f;

    // (1 - |n|)^2 在零交叉處形成尖脊
    float ridge = 1.0f - std::abs(mountain);
    ridge = std::clamp(ridge, 0.0f, 1.0f);
    ridge *= ridge;

    // 海平面附近的起伏會讓陸地反覆穿過 0，海岸碎成水窪；越接近海岸線起伏越小
    float coastMask = smoothStep(0.0f, 0.35f, std::abs(land));
    float detail = _detailNoise.fbm(worldX / DETAIL_SCALE, worldZ / DETAIL_SCALE, 6);

    return base + mountainMask * 25.0f * ridge + coastMask * 6.0f * detail;
}

}  // namespace robot::world::chunk
