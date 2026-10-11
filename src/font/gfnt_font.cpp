#include "font/gfnt_font.hpp"

#include <algorithm>
#include <utility>

namespace robot {

GfntFont::GfntFont(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> bitmapData)
    : _metrics(metrics), _glyphs(std::move(glyphs)), _bitmapData(std::move(bitmapData)) {}

const GfntFont::Glyph* GfntFont::findGlyph(char32_t codepoint) const {
    // Glyph Table 在載入時已確認為 codepoint 遞增排列。
    const auto it = std::lower_bound(_glyphs.begin(), _glyphs.end(), codepoint,
                                     [](const Glyph& glyph, char32_t searchedCodepoint) {
                                         return glyph.codepoint < searchedCodepoint;
                                     });

    if (it == _glyphs.end() || it->codepoint != codepoint) {
        return nullptr;
    }

    return &*it;
}

const GfntFont::Glyph* GfntFont::findGlyphOrFallback(char32_t codepoint) const {
    if (const auto* glyph = findGlyph(codepoint)) {
        return glyph;
    }

    if (const auto* glyph = findGlyph(U'\uFFFD')) {
        return glyph;
    }

    return findGlyph(U'?');
}

}  // namespace robot
