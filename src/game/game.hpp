#pragma once

#include <memory>
#include <optional>

#include "engine/application.hpp"
#include "engine/engine.hpp"
#include "game/screen.hpp"
#include "game/world.hpp"
#include "render/canvas.hpp"

namespace robot::game {

// 遊戲的流程：目前的世界與畫面，兩者都可以為空。
//
// 切換世界與畫面的操作延後到目前的事件或 tick 處理完畢才生效：
// 這些操作通常由畫面上的按鈕觸發，立即替換會在畫面的成員函式執行中銷毀畫面。
class Game : public engine::Application {
   public:
    explicit Game(engine::Engine& engine);
    ~Game() override;

    bool hasWorld() const { return _world != nullptr; }

    void startWorld();  // 建立新世界並關閉畫面
    void quit();

    void setScreen(std::unique_ptr<Screen> screen);  // nullptr 表示關閉畫面

    // engine::Application

    void tick(float dt, const engine::Input& input) override;
    void render() override;

    void onResize(int width, int height) override;
    void onMouseDown(MouseButton button, Point position) override;
    void onMouseUp(MouseButton button, Point position) override;
    void onMouseMove(Point position) override;

   private:
    engine::Engine& _engine;

    std::unique_ptr<World> _world;
    std::unique_ptr<Screen> _screen;

    bool _pendingStartWorld = false;
    std::optional<std::unique_ptr<Screen>> _pendingScreen;  // 內含 nullptr 表示要關閉畫面

    void applyPendingChanges();
    void renderHud(render::Canvas& canvas);
};

}  // namespace robot::game
