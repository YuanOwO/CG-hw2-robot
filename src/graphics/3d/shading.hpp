#pragma once

#include "math/vec3.hpp"

namespace robot::graphics {

/// 依法向量決定的亮度倍率，讓不同方向的面明暗不同，看得出立體感
/// @param normal 單位法向量
/// @return 乘在顏色上的倍率，介於 0 與 1 之間
float shadeOf(const Vec3& normal);

}  // namespace robot::graphics
