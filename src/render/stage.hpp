#pragma once

#include "render/camera.hpp"

namespace robot::render {

// 切換到 3D 繪製：整個視窗為繪圖區域，套用平行投影與相機，開啟深度測試。
// 投影範圍依視窗寬高比放寬（見 OrthoProjection::fittedTo），畫面不會變形。
void begin3D(int width, int height, const Camera& camera, const OrthoProjection& projection);

// 切換到 2D 繪製：整個視窗為繪圖區域，座標單位為像素、左上為原點、y 向下，關閉深度測試。
void begin2D(int width, int height);

}  // namespace robot::render
