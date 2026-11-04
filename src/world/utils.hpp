#pragma once

#include <cstdint>

#include "common/color.hpp"
#include "math/random.hpp"
#include "math/utils.hpp"

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
    auto xInt = static_cast<std::int64_t>(std::round(x));
    auto yInt = static_cast<std::int64_t>(std::round(y));
    auto zInt = static_cast<std::int64_t>(std::round(z));
    auto r = Random::uniformFloat(random.valueAt3D(xInt, yInt, zInt, fnv1a32("COLOR_R")), 0.0f, 1.0f);
    auto g = Random::uniformFloat(random.valueAt3D(xInt, yInt, zInt, fnv1a32("COLOR_G")), 0.0f, 1.0f);
    auto b = Random::uniformFloat(random.valueAt3D(xInt, yInt, zInt, fnv1a32("COLOR_B")), 0.0f, 1.0f);
    return ColorRGBA(r, g, b);
}

}  // namespace robot::world
