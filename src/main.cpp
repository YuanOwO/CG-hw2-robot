#include "engine/engine.hpp"
#include "game/game.hpp"

int main(int argc, char** argv) {
    robot::engine::Engine engine(argc, argv, "A Moving Robot", 960, 720);
    robot::game::Game game(engine);

    engine.run(game);

    return 0;
}
