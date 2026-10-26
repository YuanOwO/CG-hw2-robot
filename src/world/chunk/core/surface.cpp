#include "world/chunk/core/surface.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::chunk {

namespace {
constexpr float SEA_LEVEL = 0.0f;

constexpr float FREEZING_TEMPERATURE = 0.3f;   // 溫度 0.3 以下會結冰
constexpr float ICED_SEA_TEMPERATURE = 0.25f;  // 海水結冰的溫度比淡水低一點
constexpr float DRY_LAPSE_RATE = 0.010f;       // 乾燥時每升高 1 公尺降低的溫度
constexpr float MOIST_LAPSE_RATE = 0.005f;     // 潮濕時每升高 1 公尺降低的溫度 (水氣凝結放熱，降溫較慢)

constexpr float SNOW_MIN_HUMIDITY = 0.3f;  // 濕度太低不會下雪
constexpr float SNOW_MAX_SLOPE = 1.2f;     // 坡度太陡積不了雪

constexpr float BEACH_MAX_LANDNESS = 0.1f;  // 沙灘的邊界，越接近 0 越靠近海岸
constexpr float BEACH_MAX_HEIGHT = 0.87f;   // 沙灘的高度不能太高
constexpr float BEACH_MAX_SLOPE = 0.4f;     // 沙灘的坡度不能太陡，否則會變成岩石

constexpr float DESERT_MAX_HUMIDITY = 0.25f;    // 沙漠的濕度上限
constexpr float DESERT_MAX_SLOPE = 0.6f;        // 沙漠的坡度上限
constexpr float DESERT_MIN_TEMPERATURE = 0.5f;  // 沙漠的溫度下限，太冷就不會是沙漠

constexpr float ROCK_MIN_SLOPE = 0.85f;

}  // namespace

SurfaceType classifySurface(SurfaceInput input) {
    const float height = input.height;
    const float slope = input.slope;
    const float landness = input.landness;
    const float humidity = input.humidity;

    const float lapseRate = lerp(DRY_LAPSE_RATE, MOIST_LAPSE_RATE, humidity);
    const float temperature = input.temperature - lapseRate * std::max(0.0f, height);

    const bool isFreezing = temperature <= FREEZING_TEMPERATURE;
    const bool isSnowy = isFreezing && humidity >= SNOW_MIN_HUMIDITY;

    // 海洋、湖泊、河流、冰面
    if (height <= input.waterLevel) {
        if (height > SEA_LEVEL && isFreezing) {
            // 海平面以上的水體是淡水
            return SurfaceType::Ice;
        } else if (temperature <= ICED_SEA_TEMPERATURE) {
            // 海洋是鹹水，冰點比淡水低一點
            return SurfaceType::Ice;
        }
        return SurfaceType::Water;
    }

    if (isSnowy && slope <= SNOW_MAX_SLOPE) {
        return SurfaceType::Snow;
    }

    // 太陡峭的坡度會露出岩石
    if (slope >= ROCK_MIN_SLOPE) {
        return SurfaceType::Rock;
    }

    // 判斷是否是沙灘或岩岸
    if (landness <= BEACH_MAX_LANDNESS && height <= BEACH_MAX_HEIGHT) {
        if (isFreezing || slope > BEACH_MAX_SLOPE) {
            return SurfaceType::Rock;
        }
        return SurfaceType::Sand;
    }

    // 沙漠的條件：乾燥、炎熱、平坦
    if (humidity <= DESERT_MAX_HUMIDITY && temperature >= DESERT_MIN_TEMPERATURE) {
        if (slope > DESERT_MAX_SLOPE) {
            return SurfaceType::Rock;
        }
        return SurfaceType::Sand;
    }

    return SurfaceType::Grass;
}

}  // namespace robot::world::chunk
