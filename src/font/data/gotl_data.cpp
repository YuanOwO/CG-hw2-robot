#include "font/data/gotl_data.hpp"

#include <utility>

#include "common/bytes.hpp"
#include "font/data/glyph_lookup.hpp"

namespace robot {

GotlData::GotlData(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> outlineData)
    : _metrics(metrics), _glyphs(std::move(glyphs)), _outlineData(std::move(outlineData)) {}

const GotlData::Glyph* GotlData::findGlyph(char32_t codepoint) const {
    return glyph_lookup::find(_glyphs, codepoint);
}

const GotlData::Glyph* GotlData::findGlyphOrFallback(char32_t codepoint) const {
    return glyph_lookup::findOrFallback(_glyphs, codepoint);
}

std::vector<GotlData::OutlineCommand> GotlData::outline(const Glyph& glyph) const {
    std::vector<OutlineCommand> commands;
    commands.reserve(static_cast<std::size_t>(glyph.opCount));

    // 載入時已驗證 ops 與點都落在 outline data 內，這裡不再檢查邊界。
    // 座標為 little-endian，逐 byte 組合，不依賴平台的 endian 與對齊。
    using bytes::readI16;

    const std::uint8_t* ops = _outlineData.data() + glyph.outlineOffset;
    const std::uint8_t* point = ops + glyph.opCount + (glyph.opCount & 1);

    for (int i = 0; i < glyph.opCount; i++) {
        OutlineCommand command{static_cast<OutlineOp>(ops[i]), {}};
        const int pointCount = command.op == OutlineOp::CURVE_TO ? _metrics.curveOrder : 1;

        for (int j = 0; j < pointCount; j++) {
            command.points[j] = {readI16(point), readI16(point + 2)};
            point += 4;
        }

        commands.push_back(command);
    }

    return commands;
}

}  // namespace robot
