#include "math/arc.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot {

namespace {

// 每段至少轉這麼多，tolerance 為 0 或極小時才不會切成無限多段
const float MIN_STEP_DEGREES = 1.0f;

float angleAt(const Arc2D& arc, float t) {
    return radians(arc.startDegrees + arc.sweepDegrees * t);
}

}  // namespace

#pragma region Arc

Arc2D Arc2D::from(const Vec2& from, const Vec2& center, float sweepDegrees) {
    const Vec2 offset = from - center;
    return {center, length(offset), degrees(std::atan2(offset.y, offset.x)), sweepDegrees};
}

#pragma endregion  // Arc

#pragma region Evaluation

Vec2 evaluate(const Arc2D& arc, float t) {
    const float angle = angleAt(arc, t);
    return arc.center + Vec2(std::cos(angle), std::sin(angle)) * arc.radius;
}

// 位置為 center + r (cos θ, sin θ)，θ = θ0 + sweep · t，對 t 微分後多乘 sweep；
// sweep 的正負號同時決定了切線往哪邊
Vec2 derivative(const Arc2D& arc, float t) {
    const float angle = angleAt(arc, t);
    return Vec2(-std::sin(angle), std::cos(angle)) * (arc.radius * radians(arc.sweepDegrees));
}

#pragma endregion  // Evaluation

#pragma region Distance

// 點所在方向落在圓弧的轉角內時，最近點在同一方向的弧上；否則最近點是兩端之一
float distance(const Vec2& point, const Arc2D& arc) {
    const Vec2 offset = point - arc.center;
    const float direction = degrees(std::atan2(offset.y, offset.x));
    // 從起點往轉動方向量到點的方向，換到 [0, 360)
    const float turned = wrapDegrees(arc.sweepDegrees >= 0.0f ? direction - arc.startDegrees
                                                                : arc.startDegrees - direction);

    if (std::abs(arc.sweepDegrees) >= 360.0f || turned <= std::abs(arc.sweepDegrees)) {
        return std::abs(length(offset) - arc.radius);
    }
    return std::min(distance(point, evaluate(arc, 0.0f)), distance(point, evaluate(arc, 1.0f)));
}

#pragma endregion  // Distance

#pragma region Subdivision

std::pair<Arc2D, Arc2D> split(const Arc2D& arc, float t) {
    const float sweep = arc.sweepDegrees * t;
    return {
        {arc.center, arc.radius, arc.startDegrees,         sweep                   },
        {arc.center, arc.radius, arc.startDegrees + sweep, arc.sweepDegrees - sweep}
    };
}

// 等角度切分。轉 θ 的弦與圓弧的最大距離為 r(1 - cos(θ/2))，令它不超過 tolerance 求出每段最多轉多少
void flatten(const Arc2D& arc, float tolerance, std::vector<CurveSample2D>& samples) {
    const float cosine = std::clamp(1.0f - tolerance / arc.radius, -1.0f, 1.0f);
    const float maxStep = std::max(degrees(2.0f * std::acos(cosine)), MIN_STEP_DEGREES);
    const int count = std::max(1, static_cast<int>(std::ceil(std::abs(arc.sweepDegrees) / maxStep)));

    for (int i = 1; i <= count; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(count);
        samples.push_back({evaluate(arc, t), t});
    }
}

#pragma endregion  // Subdivision

}  // namespace robot
