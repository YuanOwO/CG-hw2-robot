#pragma once

#include <cstdint>

#include "math/vec3.hpp"

namespace robot::world {

class World;

namespace entity {
using EntityId = std::uint32_t;
class Entity;

using Age = std::uint32_t;

/// 世界座標的視線，與身體朝向無關：身體轉向時視線不跟著轉
struct Look {
    float yaw = 0.0f;    ///< 度，0 朝 +z，正值往左轉
    float pitch = 0.0f;  ///< 度，正值往上看
};

}  // namespace entity

}  // namespace robot::world
