#include "font/sized_font.hpp"

#include <algorithm>
#include <cstddef>

#include "common/utf8.hpp"
#include "font/utils.hpp"

namespace robot {

TextLayout SizedFont::layout(const std::string& text) const {
    TextLayout result{0.0f, measureHeight(text), {}};
    if (text.empty()) {
        return result;
    }

    const FontMetrics fontMetrics = metrics();

    // pen 位置以整數累加，字形的位移與寬度也都是整數像素，排版結果不會累積浮點誤差。
    int penX = 0;
    int baselineY = fontMetrics.ascender;
    int maxLineWidth = 0;

    std::size_t offset = 0;
    while (offset < text.size()) {
        const auto decoded = utf8::decodeOne(text, offset);
        if (decoded.bytesConsumed == 0) {
            break;
        }
        offset += decoded.bytesConsumed;

        if (decoded.codepoint == U'\n') {
            maxLineWidth = std::max(maxLineWidth, penX);
            penX = 0;
            baselineY += fontMetrics.lineHeight();
            continue;
        }

        const auto found = glyph(decoded.codepoint);
        if (!found) {
            penX += fontMetrics.pixelSize;
            continue;
        }

        if (found->width != 0 && found->height != 0) {
            result.glyphs.push_back(PlacedGlyph{
                static_cast<float>(penX + found->bearingX),
                static_cast<float>(baselineY - found->bearingY),
                *found,
            });
        }

        penX += found->advanceX;
    }

    result.width = static_cast<float>(std::max(maxLineWidth, penX));
    return result;
}

float SizedFont::measureWidth(const std::string& text) const {
    return layout(text).width;
}

float SizedFont::measureHeight(const std::string& text) const {
    // 只需要行數與行高，不必查字形，GOTL 字型因此不會為了量高度而點陣化。
    const int lines = countLines(text);
    return lines == 0 ? 0.0f : static_cast<float>(lines * metrics().lineHeight());
}

}  // namespace robot
