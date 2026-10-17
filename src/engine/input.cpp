#include "engine/input.hpp"

#include <utility>

namespace robot::engine {

bool Input::keyDown(Key key) {
    // 按住時的自動重複不算剛按下
    const bool firstPress = _keyboard.press(key);
    if (firstPress) {
        _pendingPressed.insert(key);
    }
    return firstPress;
}

void Input::keyUp(Key key) {
    if (_keyboard.release(key)) {
        _pendingReleased.insert(key);
    }
}

void Input::mouseDown(MouseButton button) {
    _mouse.press(button);
}

void Input::mouseUp(MouseButton button) {
    _mouse.release(button);
}

void Input::moveMouse(int x, int y) {
    _mouse.setPosition(x, y);
}

void Input::clear() {
    _keyboard.clear();
    _mouse.clear();
    _pendingPressed.clear();
    _pendingReleased.clear();
}

void Input::clearMouseButtons() {
    _mouse.clear();
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
