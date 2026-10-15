#pragma once

#include <utility>
#include <vector>

#include "math/curve_sample.hpp"
#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot {

#pragma region Segments

struct LineSegment2D {
    Vec2 start, end;
};

struct LineSegment3D {
    Vec3 start, end;
};

#pragma endregion  // Segments

#pragma region Evaluation

// 參數 t \in [0, 1]

Vec2 evaluate(const LineSegment2D& segment, float t);
Vec3 evaluate(const LineSegment3D& segment, float t);

// 與 t 無關，為 end - start
Vec2 derivative(const LineSegment2D& segment, float t);
Vec3 derivative(const LineSegment3D& segment, float t);

// 單位切線方向，與 t 無關；長度為 0 時是零向量
Vec2 tangent(const LineSegment2D& segment, float t);
Vec3 tangent(const LineSegment3D& segment, float t);

#pragma endregion  // Evaluation

#pragma region Distance

// 點到線段的最短距離
float distance(const Vec2& point, const LineSegment2D& segment);
float distance(const Vec3& point, const LineSegment3D& segment);

#pragma endregion  // Distance

#pragma region Subdivision

// 在 t 處切成兩段，交界為 evaluate(segment, t)
std::pair<LineSegment2D, LineSegment2D> split(const LineSegment2D& segment, float t);
std::pair<LineSegment3D, LineSegment3D> split(const LineSegment3D& segment, float t);

// 只把終點加到 samples（不含 start）
void flatten(const LineSegment2D& segment, float tolerance, std::vector<CurveSample2D>& samples);
void flatten(const LineSegment3D& segment, float tolerance, std::vector<CurveSample3D>& samples);

#pragma endregion  // Subdivision

}  // namespace robot
