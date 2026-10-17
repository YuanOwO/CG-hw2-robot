#pragma once

#include <unordered_set>

#include "input/input_state.hpp"
#include "input/input_types.hpp"

namespace robot::engine {

class Engine;

// 目前按住的鍵，以及這個 tick 內剛按下 / 剛放開的鍵。
// 剛按下 / 剛放開會暫存到下一個 tick 才生效，且只在該 tick 內回報。
class Input {
    friend class Engine;

   public:
    const KeyboardState& keyboard() const { return _keyboard; }
    const MouseState& mouse() const { return _mouse; }

    bool pressed(Key key) const { return _pressed.count(key) > 0; }
    bool released(Key key) const { return _released.count(key) > 0; }
    // 這個 tick 期間任何時刻按住過：兩個 tick 之間按下又放開的短按也算，至少產生一個 tick 的效果
    bool held(Key key) const { return _keyboard.isDown(key) || pressed(key); }

   private:
    bool keyDown(Key key);  // 回傳是否為第一次按下，自動重複時為 false
    void keyUp(Key key);

    void mouseDown(MouseButton button);
    void mouseUp(MouseButton button);
    void moveMouse(int x, int y);

    void clear();              // 收不到放開事件時（例如視窗被隱藏）清除，避免卡鍵
    void clearMouseButtons();

    void beginTick();
    void endTick();

    KeyboardState _keyboard;
    MouseState _mouse;

    std::unordered_set<Key> _pendingPressed;
    std::unordered_set<Key> _pendingReleased;
    std::unordered_set<Key> _pressed;
    std::unordered_set<Key> _released;
};

}  // namespace robot::engine
