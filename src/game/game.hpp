#pragma once

#include <memory>
#include <optional>

#include "engine/application.hpp"
#include "engine/engine.hpp"
#include "game/session.hpp"
#include "screen/screen.hpp"
#include "screen/screen_host.hpp"

namespace robot::game {

// 遊戲的流程：目前的 Session 與畫面，兩者都可以為空。有畫面時 Session 暫停。
//
// 切換 Session 與畫面的操作延後到目前的事件或 tick 處理完畢才生效：
// 這些操作通常由畫面上的按鈕觸發，立即替換會在畫面的成員函式執行中銷毀畫面。
class Game : public engine::Application, public screen::ScreenHost {
   public:
    explicit Game(engine::Engine& engine);
    ~Game() override;

    // screen::ScreenHost

    bool hasWorld() const override { return _session != nullptr; }

    void setScreen(std::unique_ptr<screen::Screen> screen) override;
    void openChildScreen(std::unique_ptr<screen::Screen> screen) override;

    void startWorld() override;
    void quitToTitle() override;
    void quit() override;

    // engine::Application

    void tick(float dt, const engine::Input& input) override;
    void render() override;

    void onResize(int width, int height) override;
    void onKeyDown(Key key, bool repeat) override;
    void onMouseDown(MouseButton button, Point position) override;
    void onMouseUp(MouseButton button, Point position) override;
    void onMouseMove(Point position) override;

   private:
    engine::Engine& _engine;

    std::unique_ptr<Session> _session;
    std::unique_ptr<screen::Screen> _screen;

    struct SessionRequest {
        std::unique_ptr<Session> session;  // nullptr 表示結束 Session
    };

    struct ScreenRequest {
        std::unique_ptr<screen::Screen> screen;  // nullptr 表示關閉畫面
        bool asChild = false;
    };

    std::optional<SessionRequest> _pendingSession;
    std::optional<ScreenRequest> _pendingScreen;

    void applyPendingRequests();
};

}  // namespace robot::game
