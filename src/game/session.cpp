#include "game/session.hpp"

#include <cmath>

namespace robot::game {

namespace {

const float TURN_SPEED = 90.0f;       // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;  // 度 / 像素
const float ZOOM_SPEED = 4.0f;        // 按住一秒放大或縮小的倍數

}  // namespace

Session::Session() : _world(_seed) {
    _cameraRig.setTarget(_world.player().eyePosition());
}

void Session::tick(float dt, const engine::Input& input) {
    // 先轉相機，指令才用這個 tick 的視線；世界走完後相機再跟上玩家的位置，畫面才不會慢一拍
    updateCamera(dt, input);
    _world.setPlayerCommand(_playerController.read(input, _cameraRig));
    _world.tick(dt);
    _cameraRig.setTarget(_world.player().eyePosition());
}

void Session::updateCamera(float dt, const engine::Input& input) {
    // 用按住的狀態而不是按下事件：按住多久就轉多少，轉速與幀率無關
    float direction = 0.0f;
    if (input.held(Key::Q)) {
        direction += 1.0f;
    }
    if (input.held(Key::E)) {
        direction -= 1.0f;
    }
    _cameraRig.turn(direction * TURN_SPEED * dt);

    const auto& mouse = input.mouse();
    if (!mouse.isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
    } else {
        if (_dragFrom) {
            const auto delta = mouse.position() - *_dragFrom;
            _cameraRig.turn(delta.x * DRAG_SENSITIVITY);
            _cameraRig.tilt(delta.y * DRAG_SENSITIVITY);
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
    _cameraRig.zoom(std::pow(ZOOM_SPEED, zoomDirection * dt));
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _cameraRig, width, height);
}

void Session::renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond) const {
    _hud.render(canvas, framesPerSecond, ticksPerSecond, _cameraRig);
}

}  // namespace robot::game
