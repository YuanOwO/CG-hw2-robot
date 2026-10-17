#include "game/camera_controller.hpp"

#include <cmath>

namespace robot::game {

namespace {

const float TURN_SPEED = 90.0f;       // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;  // 度 / 像素
const float ZOOM_SPEED = 4.0f;        // 按住一秒放大或縮小的倍數

}  // namespace

void CameraController::update(float dt, const engine::Input& input, CameraRig& camera) {
    // 用按住的狀態而不是按下事件：按住多久就轉多少，轉速與幀率無關
    float direction = 0.0f;
    if (input.held(Key::Q)) {
        direction += 1.0f;
    }
    if (input.held(Key::E)) {
        direction -= 1.0f;
    }
    camera.turn(direction * TURN_SPEED * dt);

    const auto& mouse = input.mouse();
    if (!mouse.isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
    } else {
        if (_dragFrom) {
            const auto delta = mouse.position() - *_dragFrom;
            camera.turn(delta.x * DRAG_SENSITIVITY);
            camera.tilt(delta.y * DRAG_SENSITIVITY);
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
    camera.zoom(std::pow(ZOOM_SPEED, zoomDirection * dt));
}

}  // namespace robot::game
