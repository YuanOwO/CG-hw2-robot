#include "io/serializer/gfnt_parser_v1.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "common/utf8.hpp"
#include "io/serializer/core/byte_reader.hpp"
#include "io/serializer/format/gfnt.hpp"

namespace robot::io {

namespace {

gfnt::GfntHeaderV1 readHeader(serializer::ByteReader& reader) {
    gfnt::GfntHeaderV1 header{};

    reader.read(header.magic, sizeof(header.magic));
    header.version = reader.u16();
    header.pixelSize = reader.u16();
    header.ascender = reader.i16();
    header.descender = reader.i16();
    header.glyphCount = reader.u32();

    return header;
}

gfnt::GfntGlyphV1 readGlyph(serializer::ByteReader& reader) {
    gfnt::GfntGlyphV1 glyph{};

    glyph.codepoint = reader.u32();
    glyph.width = reader.u16();
    glyph.height = reader.u16();
    glyph.bearingX = reader.i16();
    glyph.bearingY = reader.i16();
    glyph.advanceX = reader.u16();
    glyph.bitmapOffset = reader.u32();

    return glyph;
}

// GfntData 不綁定檔案版本，之後的新版 parser 也轉成同樣的型別。
GfntData::Glyph toGlyph(const gfnt::GfntGlyphV1& glyph) {
    return GfntData::Glyph{
        static_cast<char32_t>(glyph.codepoint),
        glyph.width,
        glyph.height,
        glyph.bearingX,
        glyph.bearingY,
        glyph.advanceX,
        glyph.bitmapOffset,
    };
}

}  // namespace

GfntData GfntParserV1::parse() {
    const std::uint64_t fileSize = inputSize();

    if (fileSize < gfnt::HEADER_SIZE) {
        throw error("file is too small");
    }

    const std::vector<std::uint8_t> headerBytes = readBytes(gfnt::HEADER_SIZE);
    serializer::ByteReader headerReader(headerBytes);
    const gfnt::GfntHeaderV1 header = readHeader(headerReader);

    if (std::memcmp(header.magic, gfnt::MAGIC, sizeof(gfnt::MAGIC)) != 0) {
        throw error("magic must be \"GFNT\"");
    }

    if (header.version != gfnt::VERSION) {
        throw error("unsupported version " + std::to_string(header.version));
    }

    if (header.pixelSize == 0) {
        throw error("pixelSize must not be zero");
    }

    if (header.ascender < 0 || header.descender < 0) {
        throw error("ascender and descender must be non-negative");
    }

    // 以 64-bit 計算，glyphCount 再大也不會溢位。
    const std::uint64_t tableSize = static_cast<std::uint64_t>(header.glyphCount) * gfnt::GLYPH_RECORD_SIZE;

    // GFNT 沒有區段位移欄位，Bitmap Data 緊接在 Glyph Table 之後。
    const std::uint64_t bitmapDataOffset = gfnt::HEADER_SIZE + tableSize;

    if (bitmapDataOffset > fileSize) {
        throw error("glyph table extends past end of file");
    }

    // 已確認 glyphCount 不超出檔案，才依它配置記憶體。
    const std::vector<std::uint8_t> tableBytes = readBytes(tableSize);
    serializer::ByteReader tableReader(tableBytes);

    std::vector<gfnt::GfntGlyphV1> glyphs;
    glyphs.reserve(header.glyphCount);

    for (std::uint32_t i = 0; i < header.glyphCount; i++) {
        glyphs.push_back(readGlyph(tableReader));
    }

    std::vector<std::uint8_t> bitmapData = readBytes(fileSize - bitmapDataOffset);

    // 載入時驗證完，繪製時就不必再檢查邊界。
    for (std::size_t i = 0; i < glyphs.size(); i++) {
        const auto& glyph = glyphs[i];
        const auto where = [&glyph] { return " (" + utf8::formatCodepoint(glyph.codepoint) + ")"; };

        // 嚴格遞增（不可重複），findGlyph() 才能二分搜尋。
        if (i != 0 && glyphs[i - 1].codepoint >= glyph.codepoint) {
            throw error("glyph table is not strictly sorted" + where());
        }

        if (!utf8::isScalarValue(glyph.codepoint)) {
            throw error("invalid Unicode code point" + where());
        }

        const std::size_t bitmapOffset = glyph.bitmapOffset;
        const std::size_t bitmapSize = static_cast<std::size_t>(glyph.width) * static_cast<std::size_t>(glyph.height);

        // 用減法比較，避免 bitmapOffset + bitmapSize 溢位。
        if (bitmapOffset > bitmapData.size() || bitmapSize > bitmapData.size() - bitmapOffset) {
            throw error("glyph bitmap extends past Bitmap Data" + where());
        }
    }

    std::vector<GfntData::Glyph> fontGlyphs;
    fontGlyphs.reserve(glyphs.size());

    for (const auto& glyph : glyphs) {
        fontGlyphs.push_back(toGlyph(glyph));
    }

    const GfntData::Metrics metrics{header.pixelSize, header.ascender, header.descender};

    return GfntData(metrics, std::move(fontGlyphs), std::move(bitmapData));
}

}  // namespace robot::io
