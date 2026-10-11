#pragma once

#include <memory>

#include "engine/application.hpp"
#include "engine/engine.hpp"
#include "game/world.hpp"

namespace robot::game {

// 遊戲的流程：持有世界，並在世界之上畫出 HUD。
class Game : public engine::Application {
   public:
    explicit Game(engine::Engine& engine);

    void tick(float dt, const engine::Input& input) override;
    void render() override;

   private:
    engine::Engine& _engine;
    std::unique_ptr<World> _world;  // 可以為空

    void renderHud();
};

}  // namespace robot::game
