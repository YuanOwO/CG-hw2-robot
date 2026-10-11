#include "game/screen.hpp"

#include <utility>

#include "game/style.hpp"

namespace robot::game {

namespace {

const int BUTTON_WIDTH = 240;
const int BUTTON_HEIGHT = 44;
const int BUTTON_SPACING = 12;

}  // namespace

Screen::Screen(Game& game) : _game(game) {}

void Screen::init(int width, int height) {
    _width = width;
    _height = height;
    _buttons.clear();
    build();
}

void Screen::render(render::Canvas& canvas) const {
    renderBackground(canvas);
    renderContent(canvas);
    for (const auto& button : _buttons) {
        button.render(canvas);
    }
}

void Screen::renderBackground(render::Canvas& canvas) const {
    canvas.fillRect(0, 0, _width, _height, style::MenuBackground);
}

bool Screen::mouseMove(Point position) {
    for (auto& button : _buttons) {
        button.mouseMove(position);
    }
    return true;
}

bool Screen::mouseDown(MouseButton button, Point position) {
    if (button != MouseButton::MouseLeft) {
        return false;
    }
    for (auto& item : _buttons) {
        if (item.mouseDown(position)) {
            return true;
        }
    }
    return false;
}

bool Screen::mouseUp(MouseButton button, Point position) {
    if (button != MouseButton::MouseLeft) {
        return false;
    }
    // 每個按鈕都要清除自己的按下狀態，因此不提早結束
    bool handled = false;
    for (auto& item : _buttons) {
        handled = item.mouseUp(position) || handled;
    }
    return handled;
}

void Screen::addButton(const BoundingBox& bounds, const std::string& label, gui::Button::PressHandler onPress) {
    _buttons.emplace_back(bounds, label, style::MenuButton, std::move(onPress));
}

BoundingBox Screen::buttonSlot(int row, int top) const {
    return BoundingBox(_width / 2 - BUTTON_WIDTH / 2, top + row * (BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH,
                       BUTTON_HEIGHT);
}

}  // namespace robot::game
