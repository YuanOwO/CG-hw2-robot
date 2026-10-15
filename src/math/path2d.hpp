#pragma once

#include <variant>
#include <vector>

#include "math/arc.hpp"
#include "math/bezier.hpp"
#include "math/line_segment.hpp"
#include "math/vec2.hpp"

namespace robot {

// 由畫筆指令組成的 2D 路徑。與 GOTL 輪廓相同，每段的起點是前一段的終點，
// 曲線的最後一個點是終點，其餘是控制點。路徑本身不閉合，要不要接回起點由使用的一方決定
class Path2D {
   public:
    using Segment = std::variant<LineSegment2D, Arc2D, QuadraticBezier2D, CubicBezier2D>;

    // 路徑切成折線後的一點。法向量是單位向量，位於前進方向的右手邊
    struct Sample {
        Vec2 position;
        Vec2 normal;
    };

    explicit Path2D(const Vec2& start);

    Path2D& lineTo(const Vec2& end);
    Path2D& quadTo(const Vec2& control, const Vec2& end);
    Path2D& cubicTo(const Vec2& control1, const Vec2& control2, const Vec2& end);
    // 從目前位置繞 center 轉 sweepDegrees，正值逆時針；半徑是目前位置到 center 的距離
    Path2D& arcAround(const Vec2& center, float sweepDegrees);
    // 沿著前一段終點的切線方向畫圓弧到 end，與前一段平滑相接。
    // 前面須至少有一段；end 落在切線上時改畫直線
    Path2D& tangentArcTo(const Vec2& end);

    const Vec2& start() const { return _start; }
    const Vec2& end() const { return _end; }  // 目前畫筆的位置，下一段的起點
    const std::vector<Segment>& segments() const { return _segments; }

    // 切成折線，折線與曲線的距離不超過 tolerance，法向量是曲線上的精確值而非由折線估計。
    // 兩段相接處切線方向不連續時（折角）在該處斷開，回傳多條折線：
    // 同一條內法向量是平滑的，不同條之間的邊是銳利的
    std::vector<std::vector<Sample>> flatten(float tolerance) const;

   private:
    Vec2 _start;
    Vec2 _end;
    std::vector<Segment> _segments;
};

}  // namespace robot
