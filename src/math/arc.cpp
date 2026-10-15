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

// 與某方向相切且通過兩點的圓只有一個。圓心 C 在通過起點 P、垂直於切線的直線上：C = P + n * s，n 為切線左手邊的單位向量。
// 由 |C - P| = |C - E| 解出 s = |E - P|² / (2 n·(E - P))；s 為正時圓心在左手邊，逆時針轉
std::optional<Arc2D> Arc2D::fromTangent(const Vec2& from, const Vec2& direction, const Vec2& to) {
    const Vec2 unit = normalize(direction);
    const Vec2 left(-unit.y, unit.x);
    const Vec2 chord = to - from;

    const float offset = dot(left, chord);
    if (unit == Vec2() || std::abs(offset) <= EPSILON * std::max(1.0f, length(chord))) {
        return std::nullopt;  // 圓的半徑為無限大
    }

    const Vec2 center = from + left * (lengthSquared(chord) / (2.0f * offset));

    // 起點與終點相對於圓心的夾角，再依轉的方向換成 (0, 360) 或 (-360, 0)
    const Vec2 start = from - center;
    const Vec2 end = to - center;
    float sweep = degrees(std::atan2(cross(start, end), dot(start, end)));
    if (offset > 0.0f && sweep < 0.0f) {
        sweep += 360.0f;
    } else if (offset < 0.0f && sweep > 0.0f) {
        sweep -= 360.0f;
    }
    return Arc2D::from(from, center, sweep);
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

// 不由導數正規化而來，半徑或轉角很小時導數太短，正規化會被當成零向量
Vec2 tangent(const Arc2D& arc, float t) {
    if (arc.radius <= 0.0f || arc.sweepDegrees == 0.0f) {
        return Vec2();
    }
    const float angle = angleAt(arc, t);
    const float turn = arc.sweepDegrees > 0.0f ? 1.0f : -1.0f;  // 逆時針時是左轉 90° 的半徑方向
    return Vec2(-std::sin(angle), std::cos(angle)) * turn;
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
