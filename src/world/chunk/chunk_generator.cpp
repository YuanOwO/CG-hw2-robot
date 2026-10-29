#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "math/utils.hpp"
#include "math/vec3.hpp"
#include "world/chunk/core/surface.hpp"
#include "world/obstacle/ball.hpp"
#include "world/obstacle/block.hpp"
#include "world/obstacle/pillar.hpp"
#include "world/utils.hpp"

namespace robot::world::chunk {
namespace {

struct VertexSample {
    ClimateSample climate;
    TerrainSample terrain;
};

}  // namespace

ChunkGenerator::ChunkGenerator(Seed seed)
    : _random(seed),
      _biomeClassifier(deriveSeed(seed, fnv1a32("BIOME"))),
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

    if (chunk->surfaceAt(3, 3) != Surface::Water && chunk->surfaceAt(3, 3) != Surface::Ice) {
        float blockX = static_cast<float>(originX) + 3.0f;
        float blockZ = static_cast<float>(originZ) + 3.0f;
        Vec3 blockPosition(blockX, chunk->heightAt(blockX, blockZ), blockZ);

        auto blockSeed = _random.valueAt2D(blockX, blockZ, fnv1a32("OBSTACLE_BLOCK"));
        Random blockRandom(blockSeed);
        float blockYaw = Random::uniformFloat(blockRandom.valueAt(fnv1a32("BLOCK_YAW")), 0.0f, 360.0f);
        Vec3 blockSize(Random::uniformFloat(blockRandom.valueAt(fnv1a32("BLOCK_SIZE_X")), 0.5f, 2.0f),
                       Random::uniformFloat(blockRandom.valueAt(fnv1a32("BLOCK_SIZE_Y")), 0.1f, 1.0f),
                       Random::uniformFloat(blockRandom.valueAt(fnv1a32("BLOCK_SIZE_Z")), 0.5f, 2.0f));

        chunk->addObstacle(std::make_unique<obstacle::Block>(blockSeed, blockPosition, blockYaw, blockSize));
    }

    if (chunk->surfaceAt(8, 8) != Surface::Water && chunk->surfaceAt(8, 8) != Surface::Ice) {
        float ballX = static_cast<float>(originX) + 8.0f;
        float ballZ = static_cast<float>(originZ) + 8.0f;
        Vec3 ballPosition(ballX, chunk->heightAt(ballX, ballZ), ballZ);

        auto ballSeed = _random.valueAt2D(ballX, ballZ, fnv1a32("OBSTACLE_BALL"));
        Random ballRandom(ballSeed);
        float ballRadius = Random::uniformFloat(ballRandom.valueAt(fnv1a32("SPHERE_RADIUS")), 0.1f, 1.0f);

        chunk->addObstacle(std::make_unique<obstacle::Ball>(ballSeed, ballPosition, ballRadius));
    }

    if (chunk->surfaceAt(13, 13) != Surface::Water && chunk->surfaceAt(13, 13) != Surface::Ice) {
        float pillarX = static_cast<float>(originX) + 13.0f;
        float pillarZ = static_cast<float>(originZ) + 13.0f;
        Vec3 pillarPosition(pillarX, chunk->heightAt(pillarX, pillarZ), pillarZ);

        auto pillarSeed = _random.valueAt2D(pillarX, pillarZ, fnv1a32("OBSTACLE_PILLAR"));
        Random pillarRandom(pillarSeed);
        float pillarRadius = Random::uniformFloat(pillarRandom.valueAt(fnv1a32("PILLAR_RADIUS")), 0.5f, 1.5f);
        float pillarHeight = Random::uniformFloat(pillarRandom.valueAt(fnv1a32("PILLAR_HEIGHT")), 0.1f, 1.0f);

        chunk->addObstacle(std::make_unique<obstacle::Pillar>(pillarSeed, pillarPosition, pillarRadius, pillarHeight));
    }

    return chunk;
}

float ChunkGenerator::heightAt(float worldX, float worldZ) const {
    auto continent = _climateSampler.continent(worldX, worldZ);
    return _terrainSampler.sample(worldX, worldZ, continent).height;
}

}  // namespace robot::world::chunk
