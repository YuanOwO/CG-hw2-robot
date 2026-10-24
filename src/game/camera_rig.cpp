#include "game/camera_rig.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::game {

namespace {

// 視線與上方向 (0, 1, 0) 平行時 gluLookAt 無法決定畫面的左右，因此不能剛好朝正上下方
const float MIN_PITCH = -89.9f;
const float MAX_PITCH = 89.9f;

// 畫面寬度最近約 4 公尺，最遠約 190 公尺
const float MIN_ZOOM = 0.5f;
const float MAX_ZOOM = 24.0f;

// 倍率為 1 時 view volume 的半寬與半高
const float BASE_HALF_WIDTH = 48.0f;
const float BASE_HALF_HEIGHT = 40.0f;

}  // namespace

void CameraRig::setYaw(float degree) {
    _yaw = wrapDegrees(degree);
}

void CameraRig::setPitch(float degree) {
    _pitch = std::clamp(degree, MIN_PITCH, MAX_PITCH);
}

void CameraRig::setDistance(float distance) {
    _distance = std::max(distance, 0.0f);
}

void CameraRig::turn(float degree) {
    _yaw = wrapDegrees(_yaw + degree);
}

void CameraRig::tilt(float degree) {
    _pitch = std::clamp(_pitch + degree, MIN_PITCH, MAX_PITCH);
}

void CameraRig::zoom(float factor) {
    _zoom = std::clamp(_zoom * factor, MIN_ZOOM, MAX_ZOOM);
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

graphics::OrthoProjection CameraRig::projection() const {
    graphics::OrthoProjection projection;
    projection.left = -BASE_HALF_WIDTH / _zoom;
    projection.right = BASE_HALF_WIDTH / _zoom;
    projection.bottom = -BASE_HALF_HEIGHT / _zoom;
    projection.top = BASE_HALF_HEIGHT / _zoom;

    // 眼睛在目標點後方 _distance 處，farPlane 要比它遠，目標點之後的地形才畫得出來
    projection.farPlane = 200.0f;
    return projection;
}

}  // namespace robot::game
