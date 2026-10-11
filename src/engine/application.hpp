#pragma once

#include "common/point.hpp"
#include "engine/input.hpp"
#include "input/input_types.hpp"

namespace robot::engine {

// 由 Engine 驅動的遊戲內容。Engine 只負責視窗與主迴圈，不知道遊戲做什麼。
//
// 兩種輸入方式並存：
//   - tick 內以 Input 查詢按鍵狀態，適合「按住就持續移動」這類操作；
//   - on* 事件在 GLUT 事件發生時立即呼叫，適合「點一下按鈕」這類只發生一次的操作。
class Application {
   public:
    virtual ~Application() = default;

    // 以固定間隔呼叫，dt 固定為 TICK_SECONDS 秒：更新遊戲狀態。
    // input 可以查詢目前按住的鍵，以及這個 tick 內剛按下 / 剛放開的鍵。
    virtual void tick(float dt, const Input& input) = 0;

    // 每幀呼叫一次：畫出目前的狀態，不應改變狀態。
    virtual void render() = 0;

    virtual void onResize(int width, int height) {}

    virtual void onKeyDown(Key key, bool repeat) {}  // repeat：按住時系統自動重複送出的按下
    virtual void onKeyUp(Key key) {}

    virtual void onMouseDown(MouseButton button, Point position) {}
    virtual void onMouseUp(MouseButton button, Point position) {}
    virtual void onMouseMove(Point position) {}
};

}  // namespace robot::engine
