#include "view/hud.hpp"

#include <cmath>
#include <string>

#include "common/color.hpp"
#include "font/font.hpp"

namespace robot::view {

namespace {

const FontStyle HUD_FONT = GlyphFontStyle{GlyphFont::UNIFONT, 16};

}  // namespace

void Hud::render(graphics::Canvas& canvas, int framesPerSecond, int ticksPerSecond) const {
    const std::string text = "FPS " + std::to_string(framesPerSecond) + " · TPS " + std::to_string(ticksPerSecond);
    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    canvas.fillRect(8, 8, textWidth + 12, textHeight + 8, ColorRGBA{0.0f, 0.0f, 0.0f, 0.6f});
    canvas.drawText(text, HUD_FONT, Color::White, Point(14.0f, 12.0f));
}

}  // namespace robot::view
