#include "engine/input.hpp"

#include <utility>

namespace robot::engine {

bool Input::keyDown(Key key) {
    // 按住時 GLUT 會重複送出按下事件；只有真正從放開變成按下才算剛按下
    const bool firstPress = _keyboard._press(key);
    if (firstPress) {
        _pendingPressed.insert(key);
    }
    return firstPress;
}

void Input::keyUp(Key key) {
    if (_keyboard._release(key)) {
        _pendingReleased.insert(key);
    }
}

void Input::mouseDown(MouseButton button) {
    _mouse._press(button);
}

void Input::mouseUp(MouseButton button) {
    _mouse._release(button);
}

void Input::moveMouse(int x, int y) {
    _mouse._setPosition(x, y);
}

void Input::clear() {
    _keyboard._clear();
    _mouse._clear();
    _pendingPressed.clear();
    _pendingReleased.clear();
}

void Input::clearMouseButtons() {
    _mouse._clear();
}

void Input::beginTick() {
    _pressed = std::move(_pendingPressed);
    _released = std::move(_pendingReleased);
    _pendingPressed.clear();
    _pendingReleased.clear();
}

void Input::endTick() {
    _pressed.clear();
    _released.clear();
}

}  // namespace robot::engine
