#include "game/player.hpp"

#include <memory>
#include <utility>

#include "math/utils.hpp"
#include "world/entity/robot.hpp"

namespace robot::game {

Player::Player(world::World& world, Vec3 position) {
    auto brain = std::make_unique<world::entity::brain::PlayerBrain>();
    _brain = brain.get();
    _entity = &world.entities().spawn<world::entity::Robot>(position, std::move(brain));

    _yaw = _entity->look().yaw;
    _pitch = _entity->look().pitch;
    syncCamera();
}

PlayerInfo Player::info() const {
    return PlayerInfo{_entity->look().yaw, _entity->look().pitch, _entity->velocity(), _entity->position()};
}

void Player::update(float dt, const engine::Input& input) {
    // 切換相機模式
    if (input.pressed(Key::Tab)) {
        switchCameraMode();
    }

    const auto turn = _cameraController.update(dt, input);
    _cameraRig.zoom(turn.zoom);

    if (_cameraMode == CameraMode::Free) {
        _cameraRig.turn(turn.yaw);
        _cameraRig.tilt(turn.pitch);
    } else {
        _yaw = wrapDegrees(_yaw + turn.yaw);
        _pitch = std::clamp(_pitch + turn.pitch, -90.0f, 90.0f);
        syncCamera();
    }

    auto intent = _playerController.read(input);
    if (_cameraMode != CameraMode::Free) {
        intent.look = {_yaw, _pitch};
    }
    if (_brain) {
        _brain->setIntent(intent);
    }
}

void Player::follow() {
    _cameraRig.setTarget(_entity->eyePosition());
}

void Player::resetCamera() {
    _cameraController.reset();
}

void Player::syncCamera() {
    _cameraRig.setTarget(_entity->eyePosition());
    switch (_cameraMode) {
    case CameraMode::FollowFace:
        _cameraRig.setYaw(_yaw);
        _cameraRig.setPitch(-_pitch);
        break;
    case CameraMode::FollowBack:
        _cameraRig.setYaw(_yaw + 180.0f);
        _cameraRig.setPitch(_pitch);
        break;
    case CameraMode::Free:
        break;
    }
}

void Player::switchCameraMode() {
    switch (_cameraMode) {
    case CameraMode::FollowFace:
        _cameraMode = CameraMode::FollowBack;
        break;
    case CameraMode::FollowBack:
        _cameraMode = CameraMode::Free;
        break;
    case CameraMode::Free:
        _cameraMode = CameraMode::FollowFace;
        break;
    }
}

}  // namespace robot::game
