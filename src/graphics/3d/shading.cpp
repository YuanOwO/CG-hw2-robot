#include "graphics/3d/shading.hpp"

namespace robot::graphics {

namespace {

const float TOP_SHADE = 1.0f;
const float BOTTOM_SHADE = 0.5f;
const float Z_SHADE = 0.8f;
const float X_SHADE = 0.6f;

}  // namespace

float shadeOf(const Vec3& normal) {
    // 以法向量分量的平方為權重：平方和為 1，軸對齊的面剛好取到單一方向的亮度
    const float vertical = normal.y > 0.0f ? TOP_SHADE : BOTTOM_SHADE;
    return normal.x * normal.x * X_SHADE + normal.y * normal.y * vertical + normal.z * normal.z * Z_SHADE;
}

}  // namespace robot::graphics
