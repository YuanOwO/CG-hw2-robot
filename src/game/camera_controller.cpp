#include "game/camera_controller.hpp"

#include <cmath>

namespace robot::game {

namespace {

const float TURN_SPEED = 90.0f;       // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;  // 度 / 像素
const float ZOOM_SPEED = 4.0f;        // 按住一秒放大或縮小的倍數

}  // namespace

CameraController::CameraTurn CameraController::update(float dt, const engine::Input& input) {
    CameraTurn result;

    // 用按住的狀態而不是按下事件：按住多久就轉多少，轉速與幀率無關
    float turnDirection = 0.0f;
    if (input.held(Key::Q)) {
        turnDirection += 1.0f;
    }
    if (input.held(Key::E)) {
        turnDirection -= 1.0f;
    }
    result.yaw += turnDirection * TURN_SPEED * dt;

    const auto& mouse = input.mouse();
    if (!mouse.isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
    } else {
        if (_dragFrom) {
            const auto delta = mouse.position() - *_dragFrom;
            result.yaw += static_cast<float>(delta.x) * DRAG_SENSITIVITY;
            result.pitch += static_cast<float>(delta.y) * DRAG_SENSITIVITY;
        }
        _dragFrom = mouse.position();
    }

    float zoomDirection = 0.0f;
    if (input.held(Key::Equal)) {
        zoomDirection += 1.0f;
    }
    if (input.held(Key::Minus)) {
        zoomDirection -= 1.0f;
    }
    // 以指數變化，不論目前的倍率，縮放的手感都一樣
    result.zoom = std::pow(ZOOM_SPEED, zoomDirection * dt);

    return result;
}

}  // namespace robot::game
