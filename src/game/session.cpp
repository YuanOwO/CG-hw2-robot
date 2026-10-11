#include "game/session.hpp"

namespace robot::game {

Session::Session() {
    // 從斜上方俯瞰整個世界
    const float center = world::WORLD_SIZE / 2.0f;
    _camera.focus = Vec3(center, 0.0f, center);
    _camera.eye = _camera.focus + Vec3(50.0f, 25.0f, 50.0f);

    // 世界的對角線斜看時寬約 90，左右各留一點邊
    _projection.left = -48.0f;
    _projection.right = 48.0f;

    // 世界最遠的角落離眼睛約 120，涵蓋到 200 留出餘裕
    _projection.farPlane = 200.0f;
}

void Session::tick(float dt) {
    _world.tick(dt);
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _camera, _projection, width, height);
}

void Session::renderHud(graphics::Canvas& canvas, int framesPerSecond, int ticksPerSecond) const {
    _hud.render(canvas, framesPerSecond, ticksPerSecond);
}

}  // namespace robot::game
