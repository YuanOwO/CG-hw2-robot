#pragma once

#include <unordered_set>

#include "input/input_state.hpp"
#include "input/input_types.hpp"

namespace robot::engine {

class Engine;

// 遊戲邏輯查詢的輸入狀態：目前按住哪些鍵，以及這個 tick 內剛按下 / 剛放開哪些鍵。
//
// GLUT 的按鍵事件隨時可能發生，不一定落在 tick 之間的固定位置。剛按下的鍵會先暫存，
// 等下一個 tick 開始時才生效，並只在那一個 tick 內回報，因此：
//   - 某一幀沒有執行 tick 時，按鍵不會遺失；
//   - 某一幀執行多個 tick 時，同一次按下不會被處理兩次。
class Input {
    friend class Engine;

   public:
    const KeyboardState& keyboard() const { return _keyboard; }
    const MouseState& mouse() const { return _mouse; }

    bool pressed(Key key) const { return _pressed.count(key) > 0; }
    bool released(Key key) const { return _released.count(key) > 0; }

   private:
    bool keyDown(Key key);  // 回傳是否為第一次按下（不是按住時的自動重複）
    void keyUp(Key key);

    void mouseDown(MouseButton button);
    void mouseUp(MouseButton button);
    void moveMouse(int x, int y);

    void clear();              // 清除所有按鍵與滑鼠按鈕，避免放開事件遺失而卡鍵
    void clearMouseButtons();  // 滑鼠進入或離開視窗時，按鈕狀態可能不一致

    void beginTick();  // 把暫存的剛按下 / 剛放開移到這個 tick
    void endTick();    // 清除這個 tick 的剛按下 / 剛放開

    KeyboardState _keyboard;
    MouseState _mouse;

    std::unordered_set<Key> _pendingPressed;
    std::unordered_set<Key> _pendingReleased;
    std::unordered_set<Key> _pressed;
    std::unordered_set<Key> _released;
};

}  // namespace robot::engine
