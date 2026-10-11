#pragma once

#include "render/render_context.hpp"

namespace robot::ui {
class InputElement;
}

namespace robot::render {

class InputRenderer {
   public:
    void render(RenderContext& context, const ui::InputElement& input);
    void renderCursor(RenderContext& context, const ui::InputElement& input);
};

}  // namespace robot::render
