#pragma once

#include <functional>

#include "common/color.hpp"
#include "ui/element.hpp"
#include "ui/theme.hpp"

namespace robot::ui {

struct ButtonStyle {
    ColorRGBA background = theme::ControlBackground;
    ColorRGBA hovered = theme::ControlHovered;
    ColorRGBA pressed = theme::ControlPressed;
    ColorRGBA border = theme::ControlBorder;
};

class ButtonElement : public Element {
   public:
    using ClickHandler = std::function<void()>;

    ButtonElement(ClickHandler onClick = nullptr);

    bool isHovered() const { return _hovered; }
    bool isPressed() const { return _pressed; }
    const ButtonStyle& style() const { return _style; }
    void setStyle(const ButtonStyle& style) {
        _style = style;
        updateVisualState();
    }

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;

   private:
    ClickHandler _onClick;
    ButtonStyle _style;

    bool _hovered = false;
    bool _pressed = false;

    // 依 hover / pressed 狀態把對應的 style 套用到 Element 的外觀屬性。
    void updateVisualState();
};

}  // namespace robot::ui
