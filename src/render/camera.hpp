#pragma once

#include "math/vec3.hpp"

namespace robot::render {

// 眼睛座標系：eye 為眼睛位置 E、focus 為注視點 F、up 為上方向 Vup，對應 gluLookAt 的三組參數。
struct Camera {
    Vec3 eye{60.0f, 60.0f, 60.0f};
    Vec3 focus{25.0f, 0.0f, 25.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
};

// 平行投影的 view volume，以眼睛座標表示，對應 glOrtho 的六個參數。
struct OrthoProjection {
    float left = -40.0f;
    float right = 40.0f;
    float bottom = -40.0f;
    float top = 40.0f;
    float nearPlane = 0.0f;
    float farPlane = 120.0f;

    // 依繪圖區域的寬高比放寬 view volume：原本的範圍完整保留，只在較寬的方向以中心向外延伸，
    // 讓畫面不因視窗比例而變形。aspect 為寬 / 高，須大於 0。
    OrthoProjection fittedTo(float aspect) const;
};

}  // namespace robot::render
