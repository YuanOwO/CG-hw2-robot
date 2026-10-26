#include "world/chunk/chunk_generator.hpp"

#include <algorithm>
#include <array>

#include "math/utils.hpp"
#include "world/chunk/core/surface.hpp"
#include "world/utils.hpp"

namespace robot::world::chunk {

ChunkGenerator::ChunkGenerator(Seed seed)
    : _seed(seed),
      _climateSampler(deriveSeed(seed, fnv1a32("CLIMATE"))),
      _terrainSampler(deriveSeed(seed, fnv1a32("TERRAIN"))) {}

std::unique_ptr<Chunk> ChunkGenerator::generate(ChunkPos pos) const {
    int originX = pos.x * CHUNK_SIZE;
    int originZ = pos.z * CHUNK_SIZE;

    Chunk::Heights heights;
    Chunk::Heights waterLevels;
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
            ClimateSample climate = _climateSampler.sample(worldX, worldZ);
            TerrainSample terrain = _terrainSampler.sample(worldX, worldZ, climate.continent);

            heights[indexOf(x, z)] = terrain.height;
            waterLevels[indexOf(x, z)] = terrain.waterLevel;
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
            float w00 = waterLevels[indexOf(x, z)];
            float w01 = waterLevels[indexOf(x, z + 1)];
            float w11 = waterLevels[indexOf(x + 1, z + 1)];
            float w10 = waterLevels[indexOf(x + 1, z)];

            SurfaceInput input;
            input.x = static_cast<float>(originX + x) + 0.5f;
            input.z = static_cast<float>(originZ + z) + 0.5f;
            input.height = cellHeight(h00, h01, h11, h10);
            input.slope = cellSlope(h00, h01, h11, h10);
            input.waterLevel = cellHeight(w00, w01, w11, w10);
            input.landness = cellAverage(landness, x, z);
            input.temperature = cellAverage(temperature, x, z);
            input.humidity = cellAverage(humidity, x, z);

            surfaces[z * CHUNK_SIZE + x] = classifySurface(input);
        }
    }

    auto chunk = std::make_unique<Chunk>(pos, heights, waterLevels, surfaces);
    return chunk;
}

float ChunkGenerator::heightAt(float worldX, float worldZ) const {
    auto continent = _climateSampler.continent(worldX, worldZ);
    return _terrainSampler.sample(worldX, worldZ, continent).height;
}

}  // namespace robot::world::chunk
