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

    if (wasArmed && _hovered && _onPress) {
        _onPress();
    }
    return wasArmed;
}

void Button::render(render::Canvas& canvas) const {
    const ColorRGBA& fill = _armed && _hovered ? _style.pressed : _hovered ? _style.hovered : _style.fill;
    const int radius = _style.cornerRadius;

    canvas.fillRoundedRect(_bounds.x + 1, _bounds.y + 1, _bounds.width - 2, _bounds.height - 2, radius - 1, fill);
    canvas.strokeRoundedRect(_bounds.x, _bounds.y, _bounds.width, _bounds.height, radius, 1, _style.border);

    const Point center(_bounds.x + _bounds.width / 2.0f, _bounds.y + _bounds.height / 2.0f);
    canvas.drawText(_label, _style.font, _style.text, center, {render::TextAlign::CENTER, render::TextAlign::CENTER});
}

}  // namespace robot::gui
