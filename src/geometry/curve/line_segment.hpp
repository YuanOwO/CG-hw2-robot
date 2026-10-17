#pragma once

#include <utility>

#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot::geometry {

/// 從 start 到 end 的線段。參數 t 從 0 到 1，與走過的距離成正比
class LineSegment2D {
   public:
    LineSegment2D(const Vec2& start, const Vec2& end);
    ~LineSegment2D() = default;

    Vec2 startPoint() const { return _start; }  ///< 起點，即 evaluate(0)
    Vec2 endPoint() const { return _end; }      ///< 終點，即 evaluate(1)

    /// 線段上的點
    /// @param t 0 為起點，1 為終點
    Vec2 evaluate(float t) const;
    /// 對 t 的導數，與 t 無關，為 end - start
    Vec2 derivative(float t) const;
    /// 單位切線方向，與 t 無關；長度為 0 時是零向量
    Vec2 tangent(float t) const;

    /// 點到線段的最短距離；長度為 0 時即為到起點的距離
    float distance(const Vec2& point) const;

    /// 在 t 處切成兩段，交界為 evaluate(t)
    std::pair<LineSegment2D, LineSegment2D> split(float t) const;
    /// 線段本身就是直的，一律為 true
    bool isFlat(float tolerance) const;

   private:
    Vec2 _start, _end;
};

/// 從 start 到 end 的線段。參數 t 從 0 到 1，與走過的距離成正比
class LineSegment3D {
   public:
    LineSegment3D(const Vec3& start, const Vec3& end);
    ~LineSegment3D() = default;

    Vec3 startPoint() const { return _start; }  ///< 起點，即 evaluate(0)
    Vec3 endPoint() const { return _end; }      ///< 終點，即 evaluate(1)

    /// 線段上的點
    /// @param t 0 為起點，1 為終點
    Vec3 evaluate(float t) const;
    /// 對 t 的導數，與 t 無關，為 end - start
    Vec3 derivative(float t) const;
    /// 單位切線方向，與 t 無關；長度為 0 時是零向量
    Vec3 tangent(float t) const;

    /// 點到線段的最短距離；長度為 0 時即為到起點的距離
    float distance(const Vec3& point) const;

    /// 在 t 處切成兩段，交界為 evaluate(t)
    std::pair<LineSegment3D, LineSegment3D> split(float t) const;
    /// 線段本身就是直的，一律為 true
    bool isFlat(float tolerance) const;

   private:
    Vec3 _start, _end;
};

}  // namespace robot::geometry
