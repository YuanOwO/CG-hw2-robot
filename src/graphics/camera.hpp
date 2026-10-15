#pragma once

#include "math/vec3.hpp"

namespace robot::graphics {

/// 觀察參數：從 eye 看向 focus，up 決定畫面的上方。由這三者建立眼睛座標系，對應 gluLookAt 的三組參數
struct Camera {
    Vec3 eye{45.0f, 70.0f, 55.0f};
    Vec3 focus{25.0f, 0.0f, 0.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
};

/// 平行投影的 view volume：眼睛座標系中的長方體，裡面的東西才會畫出來，對應 glOrtho 的六個參數
struct OrthoProjection {
    float left = -40.0f;
    float right = 40.0f;
    float bottom = -40.0f;
    float top = 40.0f;

    float nearPlane = 0.0f;
    float farPlane = 120.0f;

    /// 依繪圖區域的寬高比放寬 view volume，畫面因此不會變形。
    /// 原本的範圍完整保留，只沿較寬的方向從中心往兩側延伸；深度範圍不變
    /// @param aspect 繪圖區域的寬高比，須大於 0
    /// @return 放寬後的 view volume
    OrthoProjection fittedTo(float aspect) const;
};

}  // namespace robot::graphics
