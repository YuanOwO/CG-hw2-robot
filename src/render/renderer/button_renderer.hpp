#pragma once

#include "render/render_context.hpp"

namespace robot::ui {
class ButtonElement;
}

namespace robot::render {

class ButtonRenderer {
   public:
    void render(RenderContext& context, const ui::ButtonElement& button);
};

}  // namespace robot::render
