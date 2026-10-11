#include "game/session.hpp"

namespace robot::game {

namespace {

const float TURN_SPEED = 90.0f;        // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;   // 度 / 像素

}  // namespace

Session::Session() {
    const float center = world::WORLD_SIZE / 2.0f;
    _cameraRig.setTarget(Vec3(center, 0.0f, center));
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

    if (!input.mouse().isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
        return;
    }

    // 從按下的那一刻才開始累計，沒按住時游標移動的距離不算進來
    const Point position = input.mouse().position();
    if (_dragFrom) {
        const Vec2 delta = position - *_dragFrom;
        // 與 Minecraft 相同：往右拖向右看、往上拖抬頭；螢幕座標的 y 向下，因此兩者都取負號
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
