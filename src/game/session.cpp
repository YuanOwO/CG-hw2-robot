#include "game/session.hpp"

namespace robot::game {

void Session::tick(float dt) {
    _world.tick(dt);
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _camera, _projection, width, height);
}

void Session::renderHud(render::Canvas& canvas, int framesPerSecond, int ticksPerSecond) const {
    _hud.render(canvas, framesPerSecond, ticksPerSecond);
}

}  // namespace robot::game
