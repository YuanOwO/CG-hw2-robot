#pragma once

#include <cstdint>

namespace robot::world {

using Seed = std::uint64_t;
using StreamId = std::uint32_t;
class World;

namespace entity {
using EntityId = std::uint32_t;
class Entity;
}  // namespace entity

}  // namespace robot::world
