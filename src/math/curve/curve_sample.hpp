#pragma once

#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot {

// 曲線切成折線後的一點，t 為它在原曲線上的參數，可再拿去求導數
struct CurveSample2D {
    Vec2 position;
    float t;
};

struct CurveSample3D {
    Vec3 position;
    float t;
};

}  // namespace robot
