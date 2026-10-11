#include "game/screen.hpp"

#include <cmath>
#include <utility>

#include "game/game.hpp"
#include "game/style.hpp"

namespace robot::game {

namespace {

const int BUTTON_WIDTH = 240;
const int BUTTON_HEIGHT = 44;
const int BUTTON_SPACING = 12;

}  // namespace

Screen::Screen(Game& game, std::string title) : _game(game), _title(std::move(title)) {}

void Screen::init(int width, int height) {
    _width = width;
    _height = height;
    _buttons.clear();
    build();

    // 改變視窗大小時保留鍵盤選擇；按鈕數量變少時才放棄
    if (_focusedButton >= static_cast<int>(_buttons.size())) {
        _focusedButton = -1;
    }
}

void Screen::render(render::Canvas& canvas) const {
    renderBackground(canvas);

    if (!_title.empty()) {
        canvas.drawText(_title, style::HeadingFont, style::Text, Point(_width / 2.0f, _height / 4.0f),
                        {render::TextAlign::CENTER, render::TextAlign::START});
    }

    renderContent(canvas);

    for (int i = 0; i < static_cast<int>(_buttons.size()); i++) {
        _buttons[i].render(canvas, i == _focusedButton);
    }
}

void Screen::renderBackground(render::Canvas& canvas) const {
    canvas.fillRect(0, 0, _width, _height, _game.hasWorld() ? style::PauseOverlay : style::MenuBackground);
}

int Screen::titleBottom() const {
    return _height / 4 + static_cast<int>(std::ceil(getFontHeight(style::HeadingFont, _title)));
}

bool Screen::keyDown(Key key, bool repeat) {
    const int count = static_cast<int>(_buttons.size());

    switch (key) {
    case Key::Escape:
        // 打開畫面的那次 Esc 若繼續按住，自動重複不應立刻把畫面關掉
        if (!repeat && closesOnEscape()) {
            onClose();
        }
        return true;
    case Key::Down:
    case Key::Tab:
        if (count > 0) {
            _focusedButton = (_focusedButton + 1) % count;
        }
        return true;
    case Key::Up:
        if (count > 0) {
            _focusedButton = _focusedButton <= 0 ? count - 1 : _focusedButton - 1;
        }
        return true;
    case Key::Enter:
    case Key::Space:
        if (!repeat && _focusedButton >= 0) {
            _buttons[_focusedButton].press();
        }
        return true;
    default:
        return false;
    }
}

void Screen::onClose() {
    _game.setScreen(takeParent());
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
