#pragma once

#include "math/vec3.hpp"

namespace robot::world::physics {

struct DriveInput {
    Vec3 targetVelocity;      ///< 單位 m/s
    float jumpHeight = 0.0f;  ///< 單位 m，跳躍高度，若為 0 則不跳
};

struct Motion {
    Vec3 velocity;          ///< 單位 m/s
    Vec3 displacement;      ///< 單位 m，dt 秒內的位移
    bool onGround = false;  ///< 是否碰到地面
};

}  // namespace robot::world::physics
