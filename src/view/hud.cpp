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

void Hud::render(graphics::Canvas& canvas, const HudInfo& info) const {
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer),
                  "FPS %.2f · TPS %.2f\n"
                  "Yaw %.1f° · Pitch %.1f°\n"
                  "X: %.2f · Y: %.2f · Z: %.2f\n"
                  "VX: %.2f · VY: %.2f · VZ: %.2f\n"
                  "Biome: %s",
                  info.framesPerSecond, info.ticksPerSecond, info.cameraYaw, info.cameraPitch, info.playerPosition.x,
                  info.playerPosition.y, info.playerPosition.z, info.playerVelocity.x, info.playerVelocity.y,
                  info.playerVelocity.z, toString(info.playerBiome).c_str());
    const std::string text = buffer;

    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    canvas.fillRect(8, 8, textWidth + 12, textHeight + 8, ColorRGBA{0.0f, 0.0f, 0.0f, 0.6f});
    canvas.drawText(text, HUD_FONT, Color::White, Point(14.0f, 12.0f));
}

}  // namespace robot::view
