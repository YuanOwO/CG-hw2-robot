#pragma once

#include <vector>

#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot {
namespace detail {

// 最多切成 2^10 段，避免 tolerance 為 0 或座標為 NaN 時無限遞迴；到達上限時誤差可能超過 tolerance
constexpr int MAX_RECURSION_DEPTH = 10;

// 不夠平就對半切，兩半各自再檢查，彎曲處自然切得較細。
// curve 是原曲線在 [t0, t1] 的那一段；對半切開時，中點的參數是兩端參數的平均。
// 只加入終點，起點已由前一段或呼叫端加入
template <typename Curve, typename CurveSample>
void flatten(const Curve& curve, float tolerance, float t0, float t1, int depth, std::vector<CurveSample>& points) {
    if (curve.isFlat(tolerance) || depth >= MAX_RECURSION_DEPTH) {
        points.push_back({curve.endPoint(), t1});
        return;
    }
    auto [left, right] = curve.split(0.5f);
    auto mid = t0 + (t1 - t0) / 2.0f;
    flatten(left, tolerance, t0, mid, depth + 1, points);
    flatten(right, tolerance, mid, t1, depth + 1, points);
}

}  // namespace detail

/// 曲線切成折線後的一點
struct CurveSample2D {
    Vec2 point;
    float t;  ///< 在原曲線上的參數，可再拿去求切線
};

/// 曲線切成折線後的一點
struct CurveSample3D {
    Vec3 point;
    float t;  ///< 在原曲線上的參數，可再拿去求切線
};

/// 把曲線切成折線，折線與曲線的距離不超過 tolerance。
/// 依序加入起點（t = 0）、各段的分界點與終點（t = 1），接在 points 原有的內容之後。
/// 連續幾條曲線寫進同一個 points 時，前一條的終點與下一條的起點會重複出現
/// @tparam Curve 須提供 startPoint()、endPoint()、split(t) 與 isFlat(tolerance)
/// @tparam CurveSample 依曲線的維度使用 CurveSample2D 或 CurveSample3D
/// @param tolerance 容許的最大距離，須大於 0
template <typename Curve, typename CurveSample>
void flatten(const Curve& curve, float tolerance, std::vector<CurveSample>& points) {
    points.push_back({curve.startPoint(), 0.0f});
    detail::flatten(curve, tolerance, 0.0f, 1.0f, 0, points);
}

}  // namespace robot
