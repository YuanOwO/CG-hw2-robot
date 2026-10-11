#include "gui/button.hpp"

#include <cmath>
#include <utility>

namespace robot::gui {

Button::Button(const BoundingBox& bounds, std::string label, const ButtonStyle& style, PressHandler onPress)
    : _bounds(bounds), _label(std::move(label)), _style(style), _onPress(std::move(onPress)) {}

bool Button::contains(Point point) const {
    return _bounds.contains(static_cast<int>(std::floor(point.x)), static_cast<int>(std::floor(point.y)));
}

void Button::mouseMove(Point position) {
    _hovered = contains(position);
}

bool Button::mouseDown(Point position) {
    _hovered = contains(position);
    _armed = _hovered;
    return _armed;
}

bool Button::mouseUp(Point position) {
    const bool wasArmed = _armed;
    _armed = false;
    _hovered = contains(position);

    if (wasArmed && _hovered) {
        press();
    }
    return wasArmed;
}

void Button::press() {
    if (_onPress) {
        _onPress();
    }
}

void Button::render(graphics::Canvas& canvas, bool focused) const {
    const ColorRGBA& fill = _armed && _hovered ? _style.pressed : _hovered ? _style.hovered : _style.fill;
    const ColorRGBA& border = focused ? _style.focusBorder : _style.border;
    const int borderWidth = focused ? 2 : 1;
    const int radius = _style.cornerRadius;

    canvas.fillRoundedRect(_bounds.x + borderWidth, _bounds.y + borderWidth, _bounds.width - 2 * borderWidth,
                           _bounds.height - 2 * borderWidth, radius - borderWidth, fill);
    canvas.strokeRoundedRect(_bounds.x, _bounds.y, _bounds.width, _bounds.height, radius, borderWidth, border);

    const Point center(_bounds.x + _bounds.width / 2.0f, _bounds.y + _bounds.height / 2.0f);
    canvas.drawText(_label, _style.font, _style.text, center,
                    {graphics::TextAlign::CENTER, graphics::TextAlign::CENTER});
}

}  // namespace robot::gui
