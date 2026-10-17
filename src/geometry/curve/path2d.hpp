#pragma once

#include <vector>

#include "geometry/curve/curve.hpp"
#include "math/vec2.hpp"

namespace robot::geometry {

/// 由畫筆指令組成的 2D 路徑：每段從畫筆目前的位置開始，畫完後畫筆移到該段的終點。路徑不會自動閉合
class Path2D {
   public:
    /// 路徑切成折線後的一點
    struct Vertex {
        Vec2 point;
        Vec2 normal;  ///< 單位向量，位於前進方向的右手邊
    };

    /// 畫筆從 start 開始，還沒有任何一段
    Path2D(Vec2 start);

    const Vec2& startPoint() const { return _start; }  ///< 路徑的起點
    const Vec2& endPoint() const { return _end; }      ///< 畫筆目前的位置，下一段的起點
    const std::vector<Curve2D>& curves() const { return _curves; }

    /// 直線畫到 end
    Path2D& lineTo(const Vec2& end);
    /// 繞 center 畫圓弧，轉到 end 所在的方向；半徑是目前位置到 center 的距離，end 不必剛好在圓上，
    /// 畫筆停在圓弧實際的終點。end 與目前位置方向相同時畫一整圈
    /// @param direction 弧的方向
    /// @note 目前位置或 end 與 center 重合時方向無法決定，須避免
    Path2D& arcTo(const Vec2& center, Vec2 end, ArcDirection direction = ArcDirection::CounterClockwise);
    /// 沿著上一段終點的切線方向畫圓弧到 end，與上一段平滑相接。
    /// 還沒有任何一段時以 +x 為起始方向；end 落在切線上時改畫直線
    Path2D& tangentArcTo(const Vec2& end);
    /// 二次貝茲曲線畫到 end
    Path2D& quadraticBezierTo(const Vec2& control, const Vec2& end);
    /// 三次貝茲曲線畫到 end
    Path2D& cubicBezierTo(const Vec2& control1, const Vec2& control2, const Vec2& end);

    /// 切成折線，折線與曲線的距離不超過 tolerance；法向量取自曲線本身，不是由折線估計。
    /// 兩段相接處切線方向相同時共用一個頂點；不同時（折角）同一位置連續出現兩次，各帶自己的法向量。
    /// 長度為 0 的段會略過。
    /// 旋轉成曲面時，剖面須在 x >= 0 的一側由下往上畫，法向量才會朝外
    std::vector<Vertex> flatten(float tolerance) const;

   private:
    Vec2 _start, _end;
    std::vector<Curve2D> _curves;
};

}  // namespace robot::geometry
