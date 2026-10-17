#pragma once

#include <utility>

#include "math/vec2.hpp"

namespace robot::geometry {

/// 二次貝茲曲線：從 p0 出發、往 p1 的方向彎、到達 p2。參數 t 從 0 到 1
class QuadraticBezier2D {
   public:
    /// @param p0 起點
    /// @param p1 控制點，曲線不一定經過
    /// @param p2 終點
    QuadraticBezier2D(const Vec2& p0, const Vec2& p1, const Vec2& p2);
    ~QuadraticBezier2D() = default;

    Vec2 startPoint() const { return _p0; }  ///< 起點，即 evaluate(0)
    Vec2 endPoint() const { return _p2; }    ///< 終點，即 evaluate(1)

    /// 曲線上的點
    /// @param t 0 為起點，1 為終點
    Vec2 evaluate(float t) const;
    /// 對 t 的導數；起點為 2 (p1 - p0)，終點為 2 (p2 - p1)
    Vec2 derivative(float t) const;
    /// 單位切線方向。控制點與端點重合時曲線是 p0 到 p2 的直線，回傳該直線的方向；
    /// p0 與 p2 重合時曲線在中點折返，那一點是零向量
    Vec2 tangent(float t) const;

    /// 在 t 處切成兩段，兩段合起來與原曲線完全重疊
    std::pair<QuadraticBezier2D, QuadraticBezier2D> split(float t) const;
    /// 用起點到終點的弦代替這段曲線時，誤差是否不超過 tolerance。
    /// 曲線位於控制點圍成的三角形內，因此以 p1 到弦的距離判斷
    bool isFlat(float tolerance) const;

   private:
    Vec2 _p0, _p1, _p2;
};

/// 三次貝茲曲線：從 p0 沿 p1 的方向出發、從 p2 的方向到達 p3。參數 t 從 0 到 1
class CubicBezier2D {
   public:
    /// @param p0 起點
    /// @param p1 起點側的控制點，決定出發的方向
    /// @param p2 終點側的控制點，決定到達的方向
    /// @param p3 終點
    CubicBezier2D(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3);
    ~CubicBezier2D() = default;

    Vec2 startPoint() const { return _p0; }  ///< 起點，即 evaluate(0)
    Vec2 endPoint() const { return _p3; }    ///< 終點，即 evaluate(1)

    /// 曲線上的點
    /// @param t 0 為起點，1 為終點
    Vec2 evaluate(float t) const;
    /// 對 t 的導數；起點為 3 (p1 - p0)，終點為 3 (p3 - p2)
    Vec2 derivative(float t) const;
    /// 單位切線方向。控制點與端點重合使端點的導數為 0 時，取端點指向下一個不重合控制點的方向；
    /// 所有點重合，或曲線中間的尖點，是零向量
    Vec2 tangent(float t) const;

    /// 在 t 處切成兩段，兩段合起來與原曲線完全重疊
    std::pair<CubicBezier2D, CubicBezier2D> split(float t) const;
    /// 用起點到終點的弦代替這段曲線時，誤差是否不超過 tolerance。
    /// 曲線位於四個控制點的凸包內，因此以 p1、p2 到弦的距離判斷
    bool isFlat(float tolerance) const;

   private:
    Vec2 _p0, _p1, _p2, _p3;
};

}  // namespace robot::geometry
