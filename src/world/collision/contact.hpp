#pragma once

#include "math/vec3.hpp"

namespace robot::world::collision {

struct Contact {
    Vec3 normal;  ///< 碰撞法線，指向 A -> B，單位向量
    float depth;  ///< 碰撞深度，正值表示 A 與 B 重疊的距離
};

}  // namespace robot::world::collision
