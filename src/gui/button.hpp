#pragma once

#include <functional>
#include <string>

#include "common/bounding_box.hpp"
#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"
#include "render/canvas.hpp"

namespace robot::gui {

struct ButtonStyle {
    FontStyle font = GlyphFontStyle{GlyphFont::UNIFONT, 16};
    ColorRGBA text;
    ColorRGBA fill;
    ColorRGBA hovered;
    ColorRGBA pressed;
    ColorRGBA border;
    int cornerRadius = 6;
};

// 按鈕。在按鈕內按下並在按鈕內放開才算點擊。
class Button {
   public:
    using PressHandler = std::function<void()>;

    Button(const BoundingBox& bounds, std::string label, const ButtonStyle& style, PressHandler onPress);

    bool contains(Point point) const;

    void mouseMove(Point position);
    bool mouseDown(Point position);  // 回傳是否已處理
    bool mouseUp(Point position);

    void render(render::Canvas& canvas) const;

   private:
    BoundingBox _bounds;
    std::string _label;
    ButtonStyle _style;
    PressHandler _onPress;

    bool _hovered = false;
    bool _armed = false;  // 在按鈕內按下，尚未放開
};

}  // namespace robot::gui
