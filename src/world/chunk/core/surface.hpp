#pragma once

namespace robot::world::chunk {

enum class SurfaceType {
    Grass,
    Sand,
    Rock,
    Snow,
    Water,
    Ice,

    Count,
};

struct SurfaceInput {
    float x;            ///< 世界座標
    float z;            ///< 世界座標
    float height;       ///< 四個角落的平均高度
    float slope;        ///< 四個角落的最大坡度
    float waterLevel;   ///< 水位高度
    float landness;     ///< 大於 0 表示陸地，越接近 0 越靠近海岸
    float temperature;  ///< [0.0, 1.0]，越大越熱
    float humidity;     ///< [0.0, 1.0]，越大越潮濕
};

float cellHeight(float h00, float h01, float h11, float h10);
float cellSlope(float h00, float h01, float h11, float h10);

SurfaceType classifySurface(SurfaceInput input);

}  // namespace robot::world::chunk
