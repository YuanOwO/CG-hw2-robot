#include "game/session.hpp"

namespace robot::game {

Session::Session() : _world(_seed), _player(_world, _world.spawnPosition()) {
    _player.follow();
    _world.setFocus(_player.info().position);
}

void Session::tick(float dt, const engine::Input& input) {
    _player.update(dt, input);
    _world.tick(dt);
    _player.follow();
    _world.setFocus(_player.info().position);
}

void Session::pause() {
    _player.resetCamera();
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _player.camera(), _player.projection(), width, height);
}

void Session::renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond) const {
    auto _playerInfo = _player.info();
    view::HudInfo info{framesPerSecond,
                       ticksPerSecond,
                       _playerInfo.yaw,
                       _playerInfo.pitch,
                       _playerInfo.velocity,
                       _playerInfo.position,
                       _world.biomeAt(_playerInfo.position.x, _playerInfo.position.z)};

    _hud.render(canvas, info);
}

}  // namespace robot::game
