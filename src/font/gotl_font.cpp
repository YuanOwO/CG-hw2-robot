#include "font/gotl_font.hpp"

#include <algorithm>
#include <utility>

#include "common/bytes.hpp"

namespace robot {

GotlFont::GotlFont(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> outlineData)
    : _metrics(metrics), _glyphs(std::move(glyphs)), _outlineData(std::move(outlineData)) {}

const GotlFont::Glyph* GotlFont::findGlyph(char32_t codepoint) const {
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

const GotlFont::Glyph* GotlFont::findGlyphOrFallback(char32_t codepoint) const {
    if (const auto* glyph = findGlyph(codepoint)) {
        return glyph;
    }

    if (const auto* glyph = findGlyph(U'�')) {
        return glyph;
    }

    return findGlyph(U'?');
}

std::vector<GotlFont::OutlineCommand> GotlFont::outline(const Glyph& glyph) const {
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
