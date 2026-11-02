#pragma once

#include <vector>

#include "common/color.hpp"
#include "geometry/curve/line_segment.hpp"
#include "geometry/mesh/mesh.hpp"
#include "math/vec3.hpp"

// 呼叫當下直接用 glBegin 畫出的輔助圖形，不產生網格。
// 座標都是目前的 MODELVIEW 座標系；用到的線寬、點大小、顏色等狀態畫完會還原

namespace robot::graphics {

/// 從原點出發的 x、y、z 軸，分別為紅、綠、藍
void drawAxes(float length);

/// 一組相同顏色與粗細的線段
/// @param width 線寬，像素，不隨縮放改變
void drawLines(const std::vector<geometry::LineSegment3D>& segments, const ColorRGBA& color, float width);

/// 一組相同顏色與大小的方形點
/// @param size 邊長，像素，不隨縮放改變
void drawPoints(const std::vector<Vec3>& points, const ColorRGBA& color, float size);

/// 四邊形，依法向量決定明暗。四點不共面時沿 ac 折成兩個三角形
/// @param a, b, c, d 從正面看為逆時針
/// @param color 基本顏色
void drawQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const ColorRGBA& color);

/// 以目前的 MODELVIEW 畫出網格。顏色取自各頂點，再依法向量決定明暗；
/// 法向量屬於網格自己的座標系，所以明暗會跟著物體轉
void drawMesh(const geometry::Mesh& mesh);

}  // namespace robot::graphics
