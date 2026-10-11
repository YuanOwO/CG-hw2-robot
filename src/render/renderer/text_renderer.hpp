#pragma once

#include <string>

#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"
#include "render/render_context.hpp"

namespace robot::ui {
class TextElement;
}

namespace robot::render {

class TextRenderer {
   public:
    void render(RenderContext& context, const ui::TextElement& text);

   private:
    static void drawText(RenderContext& context, const std::string& text, const FontStyle& fontStyle,
                         const ColorRGBA& color, const Point& origin);
};

}  // namespace robot::render
