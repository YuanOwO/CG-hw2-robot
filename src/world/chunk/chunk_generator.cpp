#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "math/utils.hpp"
#include "world/chunk/core/surface.hpp"
#include "world/utils.hpp"

namespace robot::world::chunk {
namespace {

struct VertexSample {
    ClimateSample climate;
    TerrainSample terrain;
};

}  // namespace

ChunkGenerator::ChunkGenerator(Seed seed)
    : _biomeClassifier(deriveSeed(seed, fnv1a32("BIOME"))),
      _climateSampler(deriveSeed(seed, fnv1a32("CLIMATE"))),
      _terrainSampler(deriveSeed(seed, fnv1a32("TERRAIN"))) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    int originX = pos.x * CHUNK_SIZE;
    int originZ = pos.z * CHUNK_SIZE;

    Chunk::Heights heights;
    Chunk::Heights waterLevels;
    Chunk::Biomes biomes;
    Chunk::Surfaces surfaces;
    std::array<VertexSample, CHUNK_VERTEX_COUNT * CHUNK_VERTEX_COUNT> vertices;

    auto indexOf = [&](int localX, int localZ) { return localZ * CHUNK_VERTEX_COUNT + localX; };

    auto cellAverage = [&](int x, int z, auto field) {
        float a = field(vertices[indexOf(x, z)]);
        float b = field(vertices[indexOf(x, z + 1)]);
        float c = field(vertices[indexOf(x + 1, z + 1)]);
        float d = field(vertices[indexOf(x + 1, z)]);
        return (a + b + c + d) / 4.0f;
    };

    auto cellSlope = [&](int x, int z) {
        float h00 = vertices[indexOf(x, z)].terrain.height;
        float h01 = vertices[indexOf(x, z + 1)].terrain.height;
        float h11 = vertices[indexOf(x + 1, z + 1)].terrain.height;
        float h10 = vertices[indexOf(x + 1, z)].terrain.height;
        const float abc = std::hypot(h11 - h01, h01 - h00);
        const float acd = std::hypot(h10 - h00, h11 - h10);
        return std::max(abc, acd);
    };

    for (int x = 0; x < CHUNK_VERTEX_COUNT; x++) {
        for (int z = 0; z < CHUNK_VERTEX_COUNT; z++) {
            float worldX = static_cast<float>(originX + x);
            float worldZ = static_cast<float>(originZ + z);

            auto climate = _climateSampler.sample(worldX, worldZ);
            auto terrain = _terrainSampler.sample(worldX, worldZ, climate.continent);

            vertices[indexOf(x, z)] = {climate, terrain};
            heights[indexOf(x, z)] = terrain.height;
            waterLevels[indexOf(x, z)] = terrain.waterLevel;
        }
    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            BiomeInput biomeInput;
            biomeInput.worldX = static_cast<float>(originX + x) + 0.5f;
            biomeInput.worldZ = static_cast<float>(originZ + z) + 0.5f;
            biomeInput.height = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.height; });
            biomeInput.waterLevel = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.waterLevel; });
            biomeInput.slope = cellSlope(x, z);
            biomeInput.continent = cellAverage(x, z, [](const VertexSample& v) { return v.climate.continent; });
            biomeInput.temperature = cellAverage(x, z, [](const VertexSample& v) { return v.climate.temperature; });
            biomeInput.humidity = cellAverage(x, z, [](const VertexSample& v) { return v.climate.humidity; });
            biomeInput.cliffMask = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.cliffMask; });
            biomeInput.hillMask = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.hillMask; });
            biomeInput.mountainMask = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.mountainMask; });
            biomeInput.ridge = cellAverage(x, z, [](const VertexSample& v) { return v.terrain.ridge; });

            biomes[z * CHUNK_SIZE + x] = _biomeClassifier.classify(biomeInput);

            SurfaceInput surfaceInput;
            surfaceInput.biome = biomes[z * CHUNK_SIZE + x];
            surfaceInput.height = biomeInput.height;
            surfaceInput.waterLevel = biomeInput.waterLevel;
            surfaceInput.slope = biomeInput.slope;

            surfaces[z * CHUNK_SIZE + x] = _surfaceResolver.resolve(surfaceInput);
        }
    }

    auto chunk = std::make_unique<Chunk>(pos, heights, waterLevels, biomes, surfaces);
    return chunk;
}

float ChunkGenerator::heightAt(float worldX, float worldZ) const {
    auto continent = _climateSampler.continent(worldX, worldZ);
    return _terrainSampler.sample(worldX, worldZ, continent).height;
}

}  // namespace robot::world::chunk
