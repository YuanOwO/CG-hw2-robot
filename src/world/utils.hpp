#pragma once

#include <cstdint>

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

}  // namespace robot::world
