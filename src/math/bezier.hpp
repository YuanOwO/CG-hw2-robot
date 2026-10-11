#pragma once

#include <utility>
#include <vector>

#include "math/vec2.hpp"

namespace robot {

struct QuadraticBezier {
    Vec2 p0, p1, p2;  // 起點、控制點、終點
};

struct CubicBezier {
    Vec2 p0, p1, p2, p3;  // 起點、控制點 1、控制點 2、終點
};

Vec2 evaluate(const QuadraticBezier& curve, float t);
Vec2 evaluate(const CubicBezier& curve, float t);

// 以 de Casteljau 演算法在 t 處將曲線切成兩段，兩段合起來與原曲線完全重疊。
std::pair<QuadraticBezier, QuadraticBezier> split(const QuadraticBezier& curve, float t);
std::pair<CubicBezier, CubicBezier> split(const CubicBezier& curve, float t);

// 以自適應細分將曲線轉成折線，折線上任一點與曲線的距離不超過 tolerance。
// 只把各段的終點依序加到 vertices（不含 p0），方便串接連續的曲線。
void flatten(const QuadraticBezier& curve, float tolerance, std::vector<Vec2>& vertices);
void flatten(const CubicBezier& curve, float tolerance, std::vector<Vec2>& vertices);

}  // namespace robot
