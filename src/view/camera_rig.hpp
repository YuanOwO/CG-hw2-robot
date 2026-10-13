#pragma once

#include "graphics/camera.hpp"
#include "math/vec3.hpp"

namespace robot::view {

// 第三人稱相機：看著一個目標點，先決定朝向，再從目標點沿視線往後退。
// 轉向時就繞著目標環繞。相機不影響模擬，因此狀態放在這裡而不放在世界中。
class CameraRig {
   public:
    void setTarget(const Vec3& target) { _target = target; }

    // 正值為從上方看逆時針，與 glRotatef 繞 y 軸的方向相同。
    void turn(float degrees);

    // 正值為抬頭，不會到達正上方或正下方。
    void tilt(float degrees);

    // 沿畫面方向水平移動目標點。距離會除以放大倍率，畫面上的移動速度因此不隨縮放改變。
    void pan(float forward, float right);

    // factor 大於 1 拉近、小於 1 拉遠。
    void zoom(float factor);

    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }

    graphics::Camera camera() const;
    graphics::OrthoProjection projection() const;

   private:
    Vec3 _target;

    // yaw 為 0 時看向 -z，與 OpenGL 眼睛座標的預設方向一致
    float _yaw = 45.0f;     // 度，[0, 360)
    float _pitch = -20.0f;  // 度，往下看為負
    float _distance = 75.0f;

    // 平行投影沒有近大遠小，縮放要縮小 view volume，改 _distance 沒有效果
    float _zoom = 4.0f;
};

}  // namespace robot::view
