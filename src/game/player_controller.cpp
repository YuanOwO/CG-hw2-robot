#include "game/player_controller.hpp"

namespace robot::game {

world::entity::PlayerCommand PlayerController::read(const engine::Input& input, const view::CameraRig& camera) {
    if (input.pressed(Key::L)) {
        _followCamera = !_followCamera;
    }

    world::entity::PlayerCommand command;

    if (input.held(Key::W)) {
        command.forward += 1.0f;
    }
    if (input.held(Key::S)) {
        command.forward -= 1.0f;
    }
    if (input.held(Key::D)) {
        command.strafe += 1.0f;
    }
    if (input.held(Key::A)) {
        command.strafe -= 1.0f;
    }

    if (_followCamera) {
        // 相機 yaw 為 0 時看向 -z，實體則面向 +z；pitch 兩者都是正值抬頭
        command.look = {camera.yaw() + 180.0f, camera.pitch()};
    }

    return command;
}

}  // namespace robot::game
