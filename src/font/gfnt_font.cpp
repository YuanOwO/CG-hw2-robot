#include "font/gfnt_font.hpp"

namespace robot {

GfntFont::GfntFont(const GfntData& data, int scale) : _data(data), _scale(scale) {}

FontMetrics GfntFont::metrics() const {
    const auto& metrics = _data.metrics();
    return FontMetrics{metrics.pixelSize * _scale, metrics.ascender * _scale, metrics.descender * _scale};
}

std::optional<Glyph> GfntFont::glyph(char32_t codepoint) const {
    const GfntData::Glyph* glyph = _data.findGlyphOrFallback(codepoint);
    if (glyph == nullptr) {
        return std::nullopt;
    }

    return Glyph{
        glyph->width * _scale,
        glyph->height * _scale,
        glyph->bearingX * _scale,
        glyph->bearingY * _scale,
        glyph->advanceX * _scale,
        glyph->width,
        glyph->height,
        _scale,
        _data.bitmap(*glyph),
    };
}

}  // namespace robot
