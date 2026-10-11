#include "engine/engine.hpp"
#include "game/game.hpp"

int main(int argc, char** argv) {
    robot::engine::Engine engine(argc, argv, "A Moving Robot", 960, 720);
    robot::game::Game game(engine);

    // 進入主迴圈，視窗關閉後返回
    engine.run(game);

    return 0;
}
