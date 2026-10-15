#pragma once

#include <utility>
#include <vector>

#include "math/curve/curve_sample.hpp"
#include "math/vec2.hpp"

namespace robot {

#pragma region Types

struct QuadraticBezier2D {
    Vec2 p0, p1, p2;  // 起點、控制點、終點
};

struct CubicBezier2D {
    Vec2 p0, p1, p2, p3;  // 起點、控制點 1、控制點 2、終點
};

#pragma endregion  // Types

#pragma region Evaluation

// 參數 t \in [0, 1]

Vec2 evaluate(const QuadraticBezier2D& curve, float t);
Vec2 evaluate(const CubicBezier2D& curve, float t);

// 對 t 的導數，控制點與端點重合時端點處可能是零向量；要取前進方向請用 tangent
Vec2 derivative(const QuadraticBezier2D& curve, float t);
Vec2 derivative(const CubicBezier2D& curve, float t);

// 單位切線方向。導數為零向量處（控制點與端點重合、尖點）仍取曲線實際前進的方向；
// 所有點重合時是零向量
Vec2 tangent(const QuadraticBezier2D& curve, float t);
Vec2 tangent(const CubicBezier2D& curve, float t);

#pragma endregion  // Evaluation

#pragma region Subdivision

// 在 t 處將曲線切成兩段，兩段合起來與原曲線完全重疊
std::pair<QuadraticBezier2D, QuadraticBezier2D> split(const QuadraticBezier2D& curve, float t);
std::pair<CubicBezier2D, CubicBezier2D> split(const CubicBezier2D& curve, float t);

// 切成折線，折線與曲線的距離不超過 tolerance。
// 只把各段的終點依序加到 samples（不含起點），方便串接連續的曲線
void flatten(const QuadraticBezier2D& curve, float tolerance, std::vector<CurveSample2D>& samples);
void flatten(const CubicBezier2D& curve, float tolerance, std::vector<CurveSample2D>& samples);

#pragma endregion  // Subdivision

}  // namespace robot
