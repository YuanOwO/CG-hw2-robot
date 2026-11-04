#include "game/camera_rig.hpp"

#include <algorithm>

#include "math/utils.hpp"

namespace robot::game {

namespace {

// 視線與上方向 (0, 1, 0) 平行時 gluLookAt 無法決定畫面的左右，因此不能剛好朝正上下方
const float MIN_PITCH = -89.9f;
const float MAX_PITCH = 89.9f;

const float MIN_ZOOM = 0.2f;
const float MAX_ZOOM = 25.0f;

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
    // (0, 0, -1) 先依 pitch 抬頭或低頭，再繞 y 軸轉 yaw
    const Vec3 forward = rotateAroundY(rotateAroundX(Vec3(0.0f, 0.0f, -1.0f), _pitch), _yaw);

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

    // 平行投影時眼睛的位置只決定裁切範圍：nearPlane 取負值，眼睛背後的高地才不會被切掉；
    // 眼睛在目標點後方 _distance 處，farPlane 要涵蓋目標點之後整個載入範圍的地形
    projection.nearPlane = -400.0f;
    projection.farPlane = 400.0f;
    return projection;
}

}  // namespace robot::game
