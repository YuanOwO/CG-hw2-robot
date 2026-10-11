#pragma once

namespace robot::engine {

// 由 Engine 驅動的遊戲內容。Engine 只負責視窗與主迴圈，不知道遊戲做什麼。
class Application {
   public:
    virtual ~Application() = default;

    // 以固定間隔呼叫，dt 固定為 TICK_SECONDS 秒：更新遊戲狀態。
    virtual void tick(float dt) = 0;

    // 每幀呼叫一次：畫出目前的狀態，不應改變狀態。
    virtual void render() = 0;
};

}  // namespace robot::engine
