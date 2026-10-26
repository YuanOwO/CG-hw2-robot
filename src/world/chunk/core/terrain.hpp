#pragma once

#include "math/perlin.hpp"
#include "math/random.hpp"

namespace robot::world::chunk {

struct TerrainSample {
    float height = 0.0f;      ///< 地表高度
    float waterLevel = 0.0f;  ///< 水位高度
    float cliffMask = 0.0f;
    float hillMask = 0.0f;
    float mountainMask = 0.0f;
    float ridge = 0.0f;  ///< 山脊遮罩，越接近 1 越靠近山脊
};

class TerrainSampler {
   public:
    explicit TerrainSampler(Seed seed);
    ~TerrainSampler() = default;

    TerrainSample sample(const float worldX, const float worldZ, const float continent) const;

   private:
    Perlin2D _warpXNoise;
    Perlin2D _warpZNoise;
    Perlin2D _roughnessNoise;
    Perlin2D _mountainNoise;
    Perlin2D _hillNoise;
    Perlin2D _detailNoise;

    struct Context {
        float worldX;
        float worldZ;
        float continent;
        float roughness;
        float inlandMask = 0.0f;
    };

    Context createContext(const float worldX, const float worldZ, const float continent) const;
    void addBaseHeight(TerrainSample& sample, const Context& ctx) const;
    void addDetail(TerrainSample& sample, const Context& ctx) const;
    void addCoast(TerrainSample& sample, const Context& ctx) const;
    void addHills(TerrainSample& sample, const Context& ctx) const;
    void addMountain(TerrainSample& sample, const Context& ctx) const;

    void warp(const float worldX, const float worldZ, float& outX, float& outZ) const;
};

}  // namespace robot::world::chunk
