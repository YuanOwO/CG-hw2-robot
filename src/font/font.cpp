#include "font/font.hpp"

#include <stdexcept>

namespace robot {

const char* glyphFontName(GlyphFont font) {
    for (const auto& [candidate, name] : GLYPH_FONT_NAMES) {
        if (candidate == font) {
            return name;
        }
    }
    throw std::invalid_argument("Unknown glyph font");
}

std::optional<GlyphFont> findGlyphFont(const std::string& name) {
    for (const auto& [font, candidate] : GLYPH_FONT_NAMES) {
        if (name == candidate) {
            return font;
        }
    }
    return std::nullopt;
}

}  // namespace robot
