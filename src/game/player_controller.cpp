#include "game/player_controller.hpp"

namespace robot::game {

world::entity::brain::Intent PlayerController::read(const engine::Input& input) {
    world::entity::brain::Intent intent;

    if (input.held(Key::W)) {
        intent.surge += 1.0f;
    }
    if (input.held(Key::S)) {
        intent.surge -= 1.0f;
    }
    if (input.held(Key::D)) {
        intent.sway += 1.0f;
    }
    if (input.held(Key::A)) {
        intent.sway -= 1.0f;
    }

    if (input.held(Key::LeftShift)) {
        intent.run = true;
    }
    if (input.held(Key::Space)) {
        intent.jump = true;
    }

    if (input.pressed(Key::Digit1)) {
        intent.stance = world::anatomy::Stance::Standing;
    }
    if (input.pressed(Key::Digit2)) {
        intent.stance = world::anatomy::Stance::Sneaking;
    }
    if (input.pressed(Key::Digit3)) {
        intent.stance = world::anatomy::Stance::Sitting;
    }

    return intent;
}

}  // namespace robot::game
