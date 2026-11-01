#include "graphics/3d/shading.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::graphics {

namespace {

/// 由仰角與方位角求出單位方向向量；世界座標取 +x 東、+y 上、-z 北
/// @param elevation 仰角（度），地平線為 0、天頂為 90
/// @param azimuth 方位角（度），北方為 0，往東順時針增加
Vec3 directionFromAngles(float elevation, float azimuth) {
    float e = radians(elevation);
    float a = radians(azimuth);

    float cosE = std::cos(e);

    return Vec3(cosE * std::sin(a), std::sin(e), -cosE * std::cos(a));
}

const Vec3 SUNLIGHT = directionFromAngles(51.46f, 125.90f);

float halfLambert(const Vec3& normal) {
    float light = 0.5f * dot(SUNLIGHT, normal) + 0.5f;
    return light * light;  // 亮度平方，讓陰影更深
}

}  // namespace

float shadeOf(const Vec3& normal) {
    static const float FLAT_LAMBERT = halfLambert(Vec3(0.0f, 1.0f, 0.0f));
    static const float MIN_SHADE = 0.32f;
    // 平地只用到 MIN_SHADE 以上範圍的 90%，留下空間讓朝太陽的坡比平地亮
    static const float FLAT_HEADROOM = 0.9f;

    float light = MIN_SHADE + (1.0f - MIN_SHADE) * halfLambert(normal) / FLAT_LAMBERT * FLAT_HEADROOM;
    return std::min(1.0f, light);
}

}  // namespace robot::graphics
