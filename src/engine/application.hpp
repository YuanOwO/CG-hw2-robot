#pragma once

#include "common/point.hpp"
#include "engine/input.hpp"
#include "input/input_types.hpp"

namespace robot::engine {

// 由 Engine 驅動的遊戲內容。輸入可在 tick 內透過 Input 查詢，或經由 on* 事件即時接收。
class Application {
   public:
    virtual ~Application() = default;

    // 每 TICK_SECONDS 秒呼叫一次，更新遊戲狀態。
    virtual void tick(float dt, const Input& input) = 0;

    // 每幀呼叫一次，不應改變遊戲狀態。
    virtual void render() = 0;

    virtual void onResize(int width, int height) {}

    virtual void onKeyDown(Key key, bool repeat) {}  // repeat：按住時的自動重複
    virtual void onKeyUp(Key key) {}

    virtual void onMouseDown(MouseButton button, Point position) {}
    virtual void onMouseUp(MouseButton button, Point position) {}
    virtual void onMouseMove(Point position) {}
};

}  // namespace robot::engine
