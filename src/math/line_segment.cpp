#include "math/line_segment.hpp"

#include <algorithm>

#include "math/utils.hpp"

namespace robot {

namespace {

// 2D 與 3D 只差在向量型別，共用同一份實作
template <typename Segment>
std::pair<Segment, Segment> splitSegment(const Segment& segment, float t) {
    const auto middle = evaluate(segment, t);
    return {
        {segment.start, middle     },
        {middle,        segment.end}
    };
}

template <typename Point, typename Segment>
float distanceToSegment(const Point& point, const Segment& segment) {
    const auto direction = segment.end - segment.start;
    const float lengthSquaredOfSegment = lengthSquared(direction);

    if (lengthSquaredOfSegment < EPSILON * EPSILON) {
        return distance(point, segment.start);  // 線段退化成一點
    }

    // 投影到線段所在直線上，再夾回端點之間
    const float t = std::clamp(dot(point - segment.start, direction) / lengthSquaredOfSegment, 0.0f, 1.0f);
    return distance(point, evaluate(segment, t));
}

}  // namespace

#pragma region Evaluation

Vec2 evaluate(const LineSegment2D& segment, float t) {
    return lerp(segment.start, segment.end, t);
}

Vec3 evaluate(const LineSegment3D& segment, float t) {
    return lerp(segment.start, segment.end, t);
}

Vec2 derivative(const LineSegment2D& segment, float) {
    return segment.end - segment.start;
}

Vec3 derivative(const LineSegment3D& segment, float) {
    return segment.end - segment.start;
}

Vec2 tangent(const LineSegment2D& segment, float) {
    return normalize(segment.end - segment.start);
}

Vec3 tangent(const LineSegment3D& segment, float) {
    return normalize(segment.end - segment.start);
}

#pragma endregion  // Evaluation

#pragma region Distance

float distance(const Vec2& point, const LineSegment2D& segment) {
    return distanceToSegment(point, segment);
}

float distance(const Vec3& point, const LineSegment3D& segment) {
    return distanceToSegment(point, segment);
}

#pragma endregion  // Distance

#pragma region Subdivision

std::pair<LineSegment2D, LineSegment2D> split(const LineSegment2D& segment, float t) {
    return splitSegment(segment, t);
}

std::pair<LineSegment3D, LineSegment3D> split(const LineSegment3D& segment, float t) {
    return splitSegment(segment, t);
}

// 直線不必細分，整段就是一條折線
void flatten(const LineSegment2D& segment, float, std::vector<CurveSample2D>& samples) {
    samples.push_back({segment.end, 1.0f});
}

void flatten(const LineSegment3D& segment, float, std::vector<CurveSample3D>& samples) {
    samples.push_back({segment.end, 1.0f});
}

#pragma endregion  // Subdivision

}  // namespace robot
