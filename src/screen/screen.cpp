#include "screen/screen.hpp"

#include <cmath>
#include <utility>

#include "screen/style.hpp"

namespace robot::screen {

namespace {

const int BUTTON_WIDTH = 240;
const int BUTTON_HEIGHT = 44;
const int BUTTON_SPACING = 12;

}  // namespace

Screen::Screen(ScreenHost& host, std::string title) : _host(host), _title(std::move(title)) {}

void Screen::init(int width, int height) {
    _width = width;
    _height = height;
    _buttons.clear();
    build();

    // 改變視窗大小時保留鍵盤選擇；按鈕數量變少時才放棄
    if (_focusedButton >= _buttons.size()) {
        _focusedButton = static_cast<std::size_t>(-1);
    }
}

void Screen::render(graphics::Canvas& canvas) const {
    renderBackground(canvas);

    if (!_title.empty()) {
        canvas.drawText(_title, style::HeadingFont, style::Text, Point(_width / 2.0f, _height / 4.0f),
                        {graphics::TextAlign::CENTER, graphics::TextAlign::START});
    }

    renderContent(canvas);

    for (int i = 0; i < static_cast<int>(_buttons.size()); i++) {
        _buttons[i].render(canvas, i == _focusedButton);
    }
}

void Screen::renderBackground(graphics::Canvas& canvas) const {
    // 有世界時半透明，讓暫停的場景仍看得到；沒有世界時後面什麼都沒畫，必須完全蓋住
    canvas.fillRect(0, 0, _width, _height, _host.hasWorld() ? style::PauseOverlay : style::MenuBackground);
}

int Screen::titleBottom() const {
    return _height / 4 + static_cast<int>(std::ceil(getFontHeight(style::HeadingFont, _title)));
}

bool Screen::keyDown(Key key, bool repeat) {
    const auto count = _buttons.size();

    // 導覽鍵即使沒有按鈕也回報已處理：畫面打開時，按鍵不應穿透到後面的遊戲
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
        // 尚未選擇時往上從最後一個開始，跟往下從第一個開始對稱
        if (count > 0) {
            _focusedButton = _focusedButton <= 0 ? count - 1 : _focusedButton - 1;
        }
        return true;
    case Key::Enter:
    case Key::Space:
        // 按住不放只算按一次，自動重複不應連續觸發按鈕
        if (!repeat && _focusedButton >= 0) {
            _buttons[_focusedButton].press();
        }
        return true;
    default:
        return false;
    }
}

void Screen::onClose() {
    _host.setScreen(takeParent());
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
    // 按鈕互不重疊，找到一個就可以停
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

}  // namespace robot::screen
