#pragma once

#include <optional>
#include <utility>

#include "math/vec2.hpp"

namespace robot {

enum class ArcDirection {
    Clockwise,
    CounterClockwise,
};

/// 圓弧：從起點繞圓心轉過一個角度。參數 t 從 0 到 1，與轉過的角度成正比
class Arc2D {
   public:
    /// @param radius 半徑，不可為負
    /// @param startDegree 起點相對於圓心的方向，0 為 +x
    /// @param sweepDegree 轉過的角度，正值逆時針
    Arc2D(const Vec2& center, float radius, float startDegree, float sweepDegree);
    ~Arc2D() = default;

    /// 從 start 繞 center 轉 sweepDegree，半徑為 start 到 center 的距離
    /// @param sweepDegree 轉過的角度，正值逆時針
    /// @return start 與 center 重合時半徑為 0，回傳 std::nullopt
    static std::optional<Arc2D> around(const Vec2& start, const Vec2& center, float sweepDegree);

    /// 從 start 繞 center 轉到 end 所在的方向。半徑由 start 決定，end 只用來決定終點的方向，不必剛好在圓上。
    /// start 與 end 方向相同時轉一整圈
    /// @param direction 弧的方向
    /// @return start 或 end 與 center 重合時方向無法決定，回傳 std::nullopt
    static std::optional<Arc2D> around(const Vec2& start, const Vec2& center, const Vec2& end,
                                       ArcDirection direction = ArcDirection::CounterClockwise);

    /// 從 start 經過 via 到 end 的圓弧，三點都在弧上
    /// @return 三點共線或其中兩點重合時沒有這樣的圓弧，回傳 std::nullopt
    static std::optional<Arc2D> through(const Vec2& start, const Vec2& via, const Vec2& end);

    /// 從 start 沿 tangent 的方向出發、到達 end 的圓弧
    /// @param direction 在 start 的前進方向，不必是單位向量
    /// @return end 落在從 start 沿 direction 延伸的直線上時沒有這樣的圓弧，回傳 std::nullopt
    static std::optional<Arc2D> tangentAtStart(const Vec2& start, const Vec2& direction, const Vec2& end);

    /// 從 start 出發、沿 tangent 的方向到達 end 的圓弧
    /// @param direction 到達 end 時的前進方向，不必是單位向量
    /// @return start 落在通過 end、沿 direction 延伸的直線上時沒有這樣的圓弧，回傳 std::nullopt
    static std::optional<Arc2D> tangentAtEnd(const Vec2& start, const Vec2& end, const Vec2& direction);

    const Vec2& center() const { return _center; }
    float radius() const { return _radius; }
    float startDegree() const { return _startDegree; }  ///< 起點相對於圓心的方向，0 為 +x
    float sweepDegree() const { return _sweepDegree; }  ///< 轉過的角度，正值逆時針

    Vec2 startPoint() const { return evaluate(0.0f); }  ///< 起點，即 evaluate(0)
    Vec2 endPoint() const { return evaluate(1.0f); }    ///< 終點，即 evaluate(1)

    /// 圓弧上的點
    /// @param t 0 為起點，1 為終點，與轉過的角度成正比
    Vec2 evaluate(float t) const;
    /// 對 t 的導數，長度為半徑 × 轉角（弧度）；沒有轉動時是零向量
    Vec2 derivative(float t) const;
    /// 單位切線方向，沿轉動的方向；半徑或轉角為 0 時是零向量
    Vec2 tangent(float t) const;

    /// 在 t 處切成兩段，兩段的圓心與半徑不變，轉角依 t 分配
    std::pair<Arc2D, Arc2D> split(float t) const;
    /// 用起點到終點的弦代替這段圓弧時，誤差是否不超過 tolerance。
    /// 誤差為弦與弧的最大距離 r (1 - cos(轉角 / 2))
    bool isFlat(float tolerance) const;

   private:
    Vec2 _center;
    float _radius;
    float _startDegree;
    float _sweepDegree;
};

}  // namespace robot
