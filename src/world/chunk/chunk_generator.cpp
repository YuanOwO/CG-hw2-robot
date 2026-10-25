#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

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

    for (int x = 0; x < CHUNK_VERTEX_COUNT; x++) {
        for (int z = 0; z < CHUNK_VERTEX_COUNT; z++) {
            float worldX = static_cast<float>(originX + x);
            float worldZ = static_cast<float>(originZ + z);
            heights[z * CHUNK_VERTEX_COUNT + x] = terrainHeight(worldX, worldZ);
        }
    }

    auto chunk = std::make_unique<Chunk>(pos, heights);
    return chunk;
}

ChunkGenerator::ClimateSample ChunkGenerator::sampleClimate(float worldX, float worldZ) const {
    ClimateSample sample{_mainNoise.fbm(worldX / CONTINENT_SCALE, worldZ / CONTINENT_SCALE, 4) * 2.0f,
                         _temperatureNoise.fbm(worldX / TEMPERATURE_SCALE, worldZ / TEMPERATURE_SCALE, 4) * 2.0f,
                         _humidityNoise.fbm(worldX / HUMIDITY_SCALE, worldZ / HUMIDITY_SCALE, 4) * 2.0f,
                         _roughnessNoise.fbm(worldX / ROUGHNESS_SCALE, worldZ / ROUGHNESS_SCALE, 2) + 0.5f};

    return sample;
}

float ChunkGenerator::heightAt(float worldX, float worldZ) const {
    return terrainHeight(worldX, worldZ);
}

float ChunkGenerator::terrainHeight(float worldX, float worldZ) const {
    ClimateSample climate = sampleClimate(worldX, worldZ);

    // 大於 0 是陸地；偏移量決定海陸比例
    float land = climate.continent + SEA_LEVEL_BIAS;

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
    float detail = _detailNoise.fbm(worldX / 32.0f, worldZ / 32.0f, 6);

    return base + mountainMask * 25.0f * ridge + coastMask * 6.0f * detail;
}

}  // namespace robot::world::chunk
