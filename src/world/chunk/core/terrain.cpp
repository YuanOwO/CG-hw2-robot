#include "world/chunk/core/terrain.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::chunk {

TerrainSampler::TerrainSampler(Seed seed)
    : _warpXNoise(seed, fnv1a32("WARP_X")),
      _warpZNoise(seed, fnv1a32("WARP_Z")),
      _roughnessNoise(seed, fnv1a32("ROUGHNESS")),
      _mountainNoise(seed, fnv1a32("MOUNTAIN")),
      _hillNoise(seed, fnv1a32("HILL")),
      _detailNoise(seed, fnv1a32("DETAIL")) {}

TerrainSample TerrainSampler::sample(const float worldX, const float worldZ, const float continent) const {
    Context ctx = createContext(worldX, worldZ, continent);

    TerrainSample sample;

    addBaseHeight(sample, ctx);
    addCoast(sample, ctx);
    addHills(sample, ctx);
    addMountain(sample, ctx);
    addDetail(sample, ctx);

    return sample;
}

#pragma region Implementation

TerrainSampler::Context TerrainSampler::createContext(const float worldX, const float worldZ,
                                                      const float continent) const {
    static constexpr float ROUGHNESS_SCALE = 512.0f;
    static constexpr int ROUGHNESS_OCTAVES = 2;

    Context ctx;
    ctx.worldX = worldX;
    ctx.worldZ = worldZ;
    ctx.continent = continent;
    ctx.roughness = _roughnessNoise.fbm(worldX / ROUGHNESS_SCALE, worldZ / ROUGHNESS_SCALE, ROUGHNESS_OCTAVES);
    ctx.roughness = ctx.roughness * 1.46f * 0.5f + 0.5f;  // [0, 1]
    ctx.inlandMask = smoothStep(0.1f, 0.7f, ctx.continent);
    return ctx;
}

void TerrainSampler::addBaseHeight(TerrainSample& sample, const Context& ctx) const {
    static constexpr float LAND_HEIGHT_SCALE = 10.0f;
    static constexpr float OCEAN_DEPTH_SCALE = 25.0f;

    // 海底往下比陸地往上陡
    if (ctx.continent >= 0.0f) {
        sample.height = ctx.continent * LAND_HEIGHT_SCALE;
    } else {
        sample.height = ctx.continent * OCEAN_DEPTH_SCALE;
    }
    sample.waterLevel = 0.0f;
}

void TerrainSampler::addCoast(TerrainSample& sample, const Context& ctx) const {
    static constexpr float CLIFF_START_THRESHOLD = 0.003f;
    static constexpr float CLIFF_RISE_WIDTH = 0.009f;
    static constexpr float CLIFF_HEIGHT_SCALE = 10.0f;
    static constexpr float CLIFF_END_THRESHOLD = CLIFF_START_THRESHOLD + CLIFF_RISE_WIDTH;

    static constexpr float CLIFF_ROUGHNESS_MIN = 0.65f;
    static constexpr float CLIFF_ROUGHNESS_MAX = 0.8f;

    if (ctx.continent <= CLIFF_START_THRESHOLD) {
        return;
    }

    // 海岸斷崖
    const float cliffMask = smoothStep(CLIFF_ROUGHNESS_MIN, CLIFF_ROUGHNESS_MAX, ctx.roughness);
    const float landMask = std::max(1.0f - ctx.inlandMask, 0.0f);
    const float cliffRise = smoothStep(CLIFF_START_THRESHOLD, CLIFF_END_THRESHOLD, ctx.continent);
    sample.height += cliffMask * CLIFF_HEIGHT_SCALE * cliffRise * landMask;

    sample.cliffMask = cliffMask * landMask;
}

void TerrainSampler::addHills(TerrainSample& sample, const Context& ctx) const {
    static constexpr float HILL_NOISE_SCALE = 64.0f;
    static constexpr int HILL_NOISE_OCTAVES = 3;
    static constexpr float HILL_HEIGHT_SCALE = 8.0f;

    static constexpr float HILL_ROUGHNESS_MIN = 0.3f;
    static constexpr float HILL_ROUGHNESS_MAX = 0.7f;

    const float hillMask = ctx.inlandMask * smoothStep(HILL_ROUGHNESS_MIN, HILL_ROUGHNESS_MAX, ctx.roughness);
    sample.hillMask = hillMask;
    if (hillMask <= 0.0f) {
        return;
    }

    float hill = _hillNoise.fbm(ctx.worldX / HILL_NOISE_SCALE, ctx.worldZ / HILL_NOISE_SCALE, HILL_NOISE_OCTAVES);
    hill = std::max(hill * 1.46f * 0.5f + 0.5f, 0.0f);  // [0, 1]，丘陵不會太高
    sample.height += hillMask * HILL_HEIGHT_SCALE * hill;
}

void TerrainSampler::addMountain(TerrainSample& sample, const Context& ctx) const {
    static constexpr float MNTN_NOISE_SCALE = 188.0f;
    static constexpr float MNTN_DETAIL_SCALE = MNTN_NOISE_SCALE / 4.0f;
    static constexpr int MNTN_NOISE_OCTAVES = 2;
    static constexpr int MNTN_DETAIL_OCTAVES = 5;

    static constexpr float MNTN_HEIGHT_SCALE = 50.0f;
    static constexpr float MNTN_DETAIL_HEIGHT_SCALE = 0.375f;

    static constexpr float MNTN_ROUGHNESS_MIN = 0.6f;
    static constexpr float MNTN_ROUGHNESS_MAX = 0.8f;

    const float mountainMask = ctx.inlandMask * smoothStep(MNTN_ROUGHNESS_MIN, MNTN_ROUGHNESS_MAX, ctx.roughness);
    sample.mountainMask = mountainMask;
    if (mountainMask <= 0.0f) {
        return;
    }

    float warpX, warpZ;
    warp(ctx.worldX, ctx.worldZ, warpX, warpZ);

    float mountain = _mountainNoise.fbm(warpX / MNTN_NOISE_SCALE, warpZ / MNTN_NOISE_SCALE, MNTN_NOISE_OCTAVES);

    // (1 - |n|)^2 在零交叉處形成尖脊
    float ridge = 1.0f - std::abs(mountain * 1.46f);
    ridge = std::clamp(ridge, 0.0f, 1.0f);
    ridge *= ridge;
    sample.ridge = ridge;

    float detail = _mountainNoise.fbm(warpX / MNTN_DETAIL_SCALE, warpZ / MNTN_DETAIL_SCALE, MNTN_DETAIL_OCTAVES);

    mountain = ridge * (1.0f + MNTN_DETAIL_HEIGHT_SCALE * detail);
    mountain = std::max(mountain, 0.0f);

    sample.height += mountainMask * MNTN_HEIGHT_SCALE * mountain;
}

void TerrainSampler::addDetail(TerrainSample& sample, const Context& ctx) const {
    static constexpr float DETAIL_NOISE_SCALE = 48.0f;
    static constexpr int DETAIL_NOISE_OCTAVES = 5;
    static constexpr float DETAIL_HEIGHT_SCALE = 6.0f;
    static constexpr float COAST_FADE_THRESHOLD = 3.5f;

    // 海平面附近的起伏會讓陸地反覆穿過 0，海岸碎成水窪；越接近海岸線起伏越小
    float coastMask = smoothStep(0.0f, COAST_FADE_THRESHOLD, std::abs(sample.height));
    if (sample.height < 0.0f) {
        coastMask = -coastMask;
    }

    float detail =
        _detailNoise.fbm(ctx.worldX / DETAIL_NOISE_SCALE, ctx.worldZ / DETAIL_NOISE_SCALE, DETAIL_NOISE_OCTAVES);
    detail = std::clamp(detail * 1.46f * 0.5f + 0.5f, 0.0f, 1.0f);  // [0, 1]

    sample.height += coastMask * DETAIL_HEIGHT_SCALE * detail;
}

#pragma endregion  // Implementation

#pragma region Helpers

void TerrainSampler::warp(const float worldX, const float worldZ, float& outX, float& outZ) const {
    static constexpr float MNTN_WARP_SCALE = 448.0f;
    static constexpr float MNTN_WARP_STRENGTH = 47.1f;

    float noiseX = _warpXNoise.fbm(worldX / MNTN_WARP_SCALE, worldZ / MNTN_WARP_SCALE, 3);
    float noiseZ = _warpZNoise.fbm(worldX / MNTN_WARP_SCALE, worldZ / MNTN_WARP_SCALE, 3);

    outX = worldX + noiseX * MNTN_WARP_STRENGTH;
    outZ = worldZ + noiseZ * MNTN_WARP_STRENGTH;
}

#pragma endregion  // Helpers

}  // namespace robot::world::chunk
