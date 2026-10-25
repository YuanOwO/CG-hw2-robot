#include "game/session.hpp"

#include "world/entity/robot.hpp"

namespace robot::game {

namespace {

// 原點的 chunk 在 World 建構時就已載入，這裡一定查得到高度
Vec3 spawnPosition(const world::World& world) {
    constexpr float x = 0.0f;
    constexpr float z = 0.0f;
    return {x, world.heightAt(x, z).value_or(0.0f), z};
}

}  // namespace

Session::Session() : _world(_seed), _player(_world, spawnPosition(_world)) {
    _player.follow();
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
    view::HudInfo info{framesPerSecond,   ticksPerSecond,       _playerInfo.yaw,
                       _playerInfo.pitch, _playerInfo.velocity, _playerInfo.position};

    _hud.render(canvas, info);
}

}  // namespace robot::game
