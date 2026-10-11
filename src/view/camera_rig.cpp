#include "view/camera_rig.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::view {

namespace {

// 注視點在地面上，抬頭到水平以上眼睛就會鑽到地面下。
// 也不能剛好朝正下方：視線與上方向 (0, 1, 0) 平行時 gluLookAt 無法決定畫面的左右。
const float MIN_PITCH = -89.9f;
const float MAX_PITCH = 89.9f;

}  // namespace

void CameraRig::turn(float degrees) {
    _yaw = std::fmod(_yaw + degrees, 360.0f);
    if (_yaw < 0.0f) {
        _yaw += 360.0f;  // fmod 保留被除數的正負號
    }
}

void CameraRig::tilt(float degrees) {
    _pitch = std::clamp(_pitch + degrees, MIN_PITCH, MAX_PITCH);
}

graphics::Camera CameraRig::camera() const {
    const float yaw = radians(_yaw);
    const float pitch = radians(_pitch);

    // (0, 0, -1) 先依 pitch 抬頭或低頭，再繞 y 軸轉 yaw
    const Vec3 forward(-std::sin(yaw) * std::cos(pitch), std::sin(pitch), -std::cos(yaw) * std::cos(pitch));

    graphics::Camera camera;
    camera.focus = _target;
    camera.eye = camera.focus - forward * _distance;
    return camera;
}

graphics::OrthoProjection CameraRig::makeProjection() {
    graphics::OrthoProjection projection;

    // 從對角方向看時世界寬約 90，左右各留一點邊；轉到其他角度時只會更窄
    projection.left = -48.0f;
    projection.right = 48.0f;

    // 世界最遠的角落離眼睛約 120，涵蓋到 200 留出餘裕
    projection.farPlane = 200.0f;
    return projection;
}

}  // namespace robot::view
