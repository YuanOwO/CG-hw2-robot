#pragma once

#include <cstdint>

#include "math/random.hpp"
#include "world/types.hpp"

namespace robot::world {

namespace detail {

inline constexpr StreamId hashStreamId(const char* str) {
    constexpr std::uint32_t OFFSET_BASIS = 2166136261u;
    constexpr std::uint32_t FNV_PRIME = 16777619u;

    std::uint32_t hash = OFFSET_BASIS;

    for (const char* p = str; *p != '\0'; p++) {
        hash ^= static_cast<unsigned char>(*p);
        hash *= FNV_PRIME;
    }

    return hash;
}

}  // namespace detail

inline constexpr StreamId CHUNK_SEED_STREAM = detail::hashStreamId("CHUNK");
inline constexpr StreamId ENTITY_SEED_STREAM = detail::hashStreamId("ENTITY");

inline Seed deriveSeed(Seed seed, StreamId stream) {
    return Random(seed).valueAt(stream);
}

inline Seed deriveSeed(Seed seed, StreamId stream, std::uint32_t index) {
    return Random(seed).valueAt(stream, index);
}

}  // namespace robot::world
