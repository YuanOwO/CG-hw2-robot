#pragma once

#include "math/vec3.hpp"

namespace robot::world::physics {

struct Motion {
    Vec3 velocity;      ///< 單位 m/s
    Vec3 displacement;  ///< 單位 m，dt 秒內的位移
};

struct MotionResult {
    Motion motion;
    bool onGround = false;  ///< 是否碰到地面
};

}  // namespace robot::world::physics
