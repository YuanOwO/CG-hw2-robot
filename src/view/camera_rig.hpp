#pragma once

#include "graphics/camera.hpp"
#include "math/vec3.hpp"

namespace robot::view {

// 第三人稱相機：看著一個目標點，先決定朝向，再從目標點沿視線往後退。
// 轉向時就繞著目標環繞。相機不影響模擬，因此狀態放在這裡而不放在世界中。
//
// 看著什麼由呼叫端決定：總覽時是世界中心，之後跟隨時換成機器人。
class CameraRig {
   public:
    void setTarget(const Vec3& target) { _target = target; }

    // 正值為從上方看逆時針，與 glRotatef 繞 y 軸的方向相同。
    void turn(float degrees);

    // 正值為抬頭。俯仰角限制在水平以下、正下方以上，見 camera_rig.cpp。
    void tilt(float degrees);

    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }

    graphics::Camera camera() const;
    const graphics::OrthoProjection& projection() const { return _projection; }

   private:
    Vec3 _target;

    // 朝向的慣例與 Obstacle::yaw 相同；yaw 為 0 時看向 -z，與 OpenGL 眼睛座標的預設方向一致
    float _yaw = 45.0f;     // 度，[0, 360)
    float _pitch = -20.0f;  // 度，往下看為負
    float _distance = 75.0f;

    graphics::OrthoProjection _projection = makeProjection();

    static graphics::OrthoProjection makeProjection();
};

}  // namespace robot::view
