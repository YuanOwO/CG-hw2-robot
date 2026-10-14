#pragma once

#include <vector>

#include "common/color.hpp"
#include "math/vec3.hpp"

// 呼叫當下直接用 glBegin 畫出的輔助圖形，不產生網格。
// 座標都是目前的 MODELVIEW 座標系；用到的線寬、點大小、顏色等狀態畫完會還原。
namespace robot::graphics {

struct LineSegment {
    Vec3 from;
    Vec3 to;
};

// 原點出發的 x、y、z 軸，分別為紅、綠、藍
void drawAxes(float length);

// width 為像素，不隨縮放改變
void drawLines(const std::vector<LineSegment>& segments, const ColorRGBA& color, float width);

// 方形的點，size 為像素，不隨縮放改變
void drawPoints(const std::vector<Vec3>& points, const ColorRGBA& color, float size);

// 四邊形，a b c d 從正面看為逆時針。四點不一定共面，因此沿 ac 切成兩個三角形，各自依法向量決定明暗
void drawQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const ColorRGBA& color);

}  // namespace robot::graphics
