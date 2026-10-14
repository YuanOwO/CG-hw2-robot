#include "game/session.hpp"

namespace robot::game {

Session::Session() : _world(_seed) {
    _cameraRig.setTarget(_world.player().eyePosition());
}

void Session::tick(float dt, const engine::Input& input) {
    // 先轉相機，指令才用這個 tick 的視線；世界走完後相機再跟上玩家的位置，畫面才不會慢一拍
    _cameraController.update(dt, input, _cameraRig);
    _world.setPlayerCommand(_playerController.read(input, _cameraRig));
    _world.tick(dt);
    _cameraRig.setTarget(_world.player().eyePosition());
}

void Session::pause() {
    _cameraController.reset();
}

void Session::renderWorld(int width, int height) const {
    _worldRenderer.render(_world, _cameraRig, width, height);
}

void Session::renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond) const {
    _hud.render(canvas, framesPerSecond, ticksPerSecond, _cameraRig);
}

}  // namespace robot::game
