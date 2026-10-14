#pragma once

#include "math/vec3.hpp"

namespace robot::graphics {

// 不開燈時依法向量決定的亮度倍率：各方向的面明暗不同，沒有光照也看得出立體感
float shadeOf(const Vec3& normal);

}  // namespace robot::graphics
