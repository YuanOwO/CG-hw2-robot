#include "view/hud.hpp"

#include <cmath>
#include <cstdio>
#include <string>

#include "common/color.hpp"
#include "font/font.hpp"

namespace robot::view {

namespace {

const FontStyle HUD_FONT = GlyphFontStyle{GlyphFont::UNIFONT, 16};

}  // namespace

void Hud::render(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond,
                 const CameraRig& camera) const {
    char buffer[96];
    std::snprintf(buffer, sizeof(buffer), "FPS %.2f · TPS %.2f\nYaw %.1f° · Pitch %.1f°", framesPerSecond,
                  ticksPerSecond, camera.yaw(), camera.pitch());
    const std::string text = buffer;

    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    canvas.fillRect(8, 8, textWidth + 12, textHeight + 8, ColorRGBA{0.0f, 0.0f, 0.0f, 0.6f});
    canvas.drawText(text, HUD_FONT, Color::White, Point(14.0f, 12.0f));
}

}  // namespace robot::view
