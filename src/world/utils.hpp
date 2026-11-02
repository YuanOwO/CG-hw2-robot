#pragma once

#include <cstdint>

#include "common/color.hpp"
#include "math/random.hpp"
#include "math/utils.hpp"
#include "world/types.hpp"

namespace robot::world {

inline constexpr Stream CHUNK_SEED_STREAM = fnv1a32("CHUNK");
inline constexpr Stream ENTITY_SEED_STREAM = fnv1a32("ENTITY");

inline Seed deriveSeed(Seed seed, Stream stream) {
    return Random(seed).valueAt(stream);
}

inline Seed deriveSeed(Seed seed, Stream stream, std::uint32_t index) {
    return Random(seed).valueAt(stream, index);
}

inline ColorRGBA randomColor(Seed seed, float x, float y, float z) {
    Random random(seed);
    auto r = Random::uniformFloat(random.valueAt3D(x, y, z, fnv1a32("COLOR_R")), 0.0f, 1.0f);
    auto g = Random::uniformFloat(random.valueAt3D(x, y, z, fnv1a32("COLOR_G")), 0.0f, 1.0f);
    auto b = Random::uniformFloat(random.valueAt3D(x, y, z, fnv1a32("COLOR_B")), 0.0f, 1.0f);
    return ColorRGBA(r, g, b);
}

}  // namespace robot::world
