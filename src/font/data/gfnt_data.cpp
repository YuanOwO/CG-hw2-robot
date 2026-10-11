#include "font/data/gfnt_data.hpp"

#include <utility>

#include "font/data/glyph_lookup.hpp"

namespace robot {

GfntData::GfntData(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> bitmapData)
    : _metrics(metrics), _glyphs(std::move(glyphs)), _bitmapData(std::move(bitmapData)) {}

const GfntData::Glyph* GfntData::findGlyph(char32_t codepoint) const {
    return glyph_lookup::find(_glyphs, codepoint);
}

const GfntData::Glyph* GfntData::findGlyphOrFallback(char32_t codepoint) const {
    return glyph_lookup::findOrFallback(_glyphs, codepoint);
}

}  // namespace robot
