#include "game/session.hpp"

#include <cmath>

namespace robot::game {

namespace {

const float TURN_SPEED = 90.0f;       // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;  // 度 / 像素
const float ZOOM_SPEED = 4.0f;        // 按住一秒放大或縮小的倍數
const float PAN_SPEED = 20.0f;        // 公尺 / 秒，倍率為 1 時

}  // namespace

Session::Session() : _world(_seed) {
    _cameraRig.setTarget(Vec3(0.0f, 0.0f, 0.0f));
}

void Session::tick(float dt, const engine::Input& input) {
    _world.tick(dt);
    updateCamera(dt, input);
}

void Session::updateCamera(float dt, const engine::Input& input) {
    // 用按住的狀態而不是按下事件：按住多久就轉多少，轉速與幀率無關
    float direction = 0.0f;
    if (input.keyboard().isDown(Key::Q)) {
        direction += 1.0f;
    }
    if (input.keyboard().isDown(Key::E)) {
        direction -= 1.0f;
    }
    _cameraRig.turn(direction * TURN_SPEED * dt);

    // x 為往右、y 為往前
    Vec2 pan;
    if (input.keyboard().isDown(Key::W)) {
        pan.y += 1.0f;
    }
    if (input.keyboard().isDown(Key::S)) {
        pan.y -= 1.0f;
    }
    if (input.keyboard().isDown(Key::D)) {
        pan.x += 1.0f;
    }
    if (input.keyboard().isDown(Key::A)) {
        pan.x -= 1.0f;
    }
    // 斜向移動才不會快 √2 倍；沒按鍵時 normalize 回傳零向量
    pan = normalize(pan) * (PAN_SPEED * dt);
    _cameraRig.pan(pan.y, pan.x);

    float zoomDirection = 0.0f;
    if (input.keyboard().isDown(Key::Equal)) {
        zoomDirection += 1.0f;
    }
    if (input.keyboard().isDown(Key::Minus)) {
        zoomDirection -= 1.0f;
    }
    // 以指數變化，不論目前的倍率，縮放的手感都一樣
    _cameraRig.zoom(std::pow(ZOOM_SPEED, zoomDirection * dt));

    if (!input.mouse().isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
        return;
    }

    // 從按下的那一刻才開始累計，沒按住時游標移動的距離不算進來
    const Point position = input.mouse().position();
    if (_dragFrom) {
        const Vec2 delta = position - *_dragFrom;
        // 往右拖向右看、往上拖抬頭；螢幕座標的 y 向下，因此兩者都取負號
        _cameraRig.turn(-delta.x * DRAG_SENSITIVITY);
        _cameraRig.tilt(-delta.y * DRAG_SENSITIVITY);
    }
    _dragFrom = position;
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _cameraRig, width, height);
}

void Session::renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond) const {
    _hud.render(canvas, framesPerSecond, ticksPerSecond, _cameraRig);
}

}  // namespace robot::game
