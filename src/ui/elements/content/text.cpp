#include "ui/elements/content/text.hpp"

#include <cmath>

#include "common/point.hpp"

namespace robot::ui {

Size TextElement::measureContent(const Size& availableSize) {
    // 使用 getFontWidth 和 getFontHeight 計算文字的需求大小
    float width = getFontWidth(_fontStyle, _text);
    float height = getFontHeight(_fontStyle, _text);

    return Size(std::ceil(width), std::ceil(height));
}

void TextElement::renderContent(render::RenderContext& context) {
    const Point origin(padding().left, padding().top);
    context.drawText(_text, _fontStyle, _color, origin);
}

}  // namespace robot::ui
