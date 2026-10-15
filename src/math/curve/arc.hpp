#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "math/curve/curve_sample.hpp"
#include "math/vec2.hpp"

namespace robot {

#pragma region Types

struct Arc2D {
    Vec2 center;
    float radius;
    float startDegrees;  // 起點相對於圓心的方向，0 為 +x
    float sweepDegrees;  // 正值逆時針

    // 從 from 繞 center 轉 sweepDegrees 的圓弧，半徑是 from 到 center 的距離
    static Arc2D from(const Vec2& from, const Vec2& center, float sweepDegrees);

    // 從 from 沿 direction 出發、通過 to 的圓弧，在 from 與 direction 相切。
    // direction 不必是單位向量；to 落在 direction 所在的直線上時沒有這樣的圓弧，回傳 std::nullopt
    static std::optional<Arc2D> fromTangent(const Vec2& from, const Vec2& direction, const Vec2& to);
};

#pragma endregion  // Types

#pragma region Evaluation

// 參數 t \in [0, 1]，與轉過的角度成正比

Vec2 evaluate(const Arc2D& arc, float t);
// 對 t 的導數，大小為 radius × sweep（弧度），沒有轉動時是零向量
Vec2 derivative(const Arc2D& arc, float t);
// 單位切線方向，沿轉動的方向；沒有轉動或半徑為 0 時是零向量
Vec2 tangent(const Arc2D& arc, float t);

#pragma endregion  // Evaluation

#pragma region Distance

float distance(const Vec2& point, const Arc2D& arc);  // 點到圓弧的最短距離

#pragma endregion  // Distance

#pragma region Subdivision

// 在 t 處切成兩段，兩段的圓心與半徑不變，轉角依 t 分配
std::pair<Arc2D, Arc2D> split(const Arc2D& arc, float t);

// 切成折線，折線與曲線的距離不超過 tolerance。
// 只把各段的終點依序加到 samples（不含起點），方便串接連續的曲線
void flatten(const Arc2D& arc, float tolerance, std::vector<CurveSample2D>& samples);

#pragma endregion  // Subdivision

}  // namespace robot
