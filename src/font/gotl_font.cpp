#include "font/gotl_font.hpp"

#include <cstdint>

#include "math/utils.hpp"

namespace robot {

GotlFont::GotlFont(const GotlData& data, int pixelSize) : _data(data), _pixelSize(pixelSize) {}

FontMetrics GotlFont::metrics() const {
    const auto& metrics = _data.metrics();

    // 無條件進位，行高才容得下點陣化時向外取整的字形 bitmap。
    const auto scaledCeil = [&](int value) {
        return static_cast<int>(ceilDiv(static_cast<std::int64_t>(value) * _pixelSize, metrics.unitsPerEm));
    };

    return FontMetrics{_pixelSize, scaledCeil(metrics.ascender), scaledCeil(metrics.descender)};
}

std::optional<Glyph> GotlFont::glyph(char32_t codepoint) const {
    const GotlData::Glyph* outline = _data.findGlyphOrFallback(codepoint);
    if (outline == nullptr) {
        return std::nullopt;
    }

    auto it = _cache.find(outline->codepoint);
    if (it == _cache.end()) {
        it = _cache.emplace(outline->codepoint, rasterizeGlyph(_data, *outline, _pixelSize)).first;
    }

    const RasterizedGlyph& glyph = it->second;
    return Glyph{
        glyph.width,    glyph.height,   glyph.bearingX, glyph.bearingY, glyph.advanceX,
        glyph.width,    glyph.height,   1,              glyph.bitmap.data(),
    };
}

}  // namespace robot
