#include "io/serializer/gfnt_parser_v1.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "common/utf8.hpp"
#include "io/serializer/byte_reader.hpp"
#include "io/serializer/gfnt.hpp"

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

GfntFont::Glyph toGlyph(const gfnt::GfntGlyphV1& glyph) {
    return GfntFont::Glyph{
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

GfntFont GfntParserV1::parse() {
    // 檔案大小在建構時就已取得，之後才能在分配 glyph table 前確認
    // header 裡的 glyphCount 沒有指向資料外。
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

    // Bitmap Data 緊接在 Glyph Table 後面，所以不需要額外的
    // section offset 欄位。
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

    // 載入時一次驗證完整個字型，渲染時就可以直接取用 bitmap，
    // 不必每畫一個字都重複做邊界檢查。
    for (std::size_t i = 0; i < glyphs.size(); i++) {
        const auto& glyph = glyphs[i];

        // findGlyph() 使用 binary search，因此這裡不只要檢查排序，
        // 也要拒絕重複的 codepoint。
        if (i != 0 && glyphs[i - 1].codepoint >= glyph.codepoint) {
            throw error("glyph table is not strictly sorted at " + utf8::formatCodepoint(glyph.codepoint));
        }

        if (!utf8::isScalarValue(glyph.codepoint)) {
            throw error("invalid Unicode code point " + utf8::formatCodepoint(glyph.codepoint));
        }

        const std::size_t bitmapOffset = glyph.bitmapOffset;
        const std::size_t bitmapSize = static_cast<std::size_t>(glyph.width) * static_cast<std::size_t>(glyph.height);

        // 用減法比較可以避免 bitmapOffset + bitmapSize 溢位。
        if (bitmapOffset > bitmapData.size() || bitmapSize > bitmapData.size() - bitmapOffset) {
            throw error("glyph bitmap extends past Bitmap Data (" + utf8::formatCodepoint(glyph.codepoint) + ")");
        }
    }

    std::vector<GfntFont::Glyph> fontGlyphs;
    fontGlyphs.reserve(glyphs.size());

    for (const auto& glyph : glyphs) {
        fontGlyphs.push_back(toGlyph(glyph));
    }

    const GfntFont::Metrics metrics{header.pixelSize, header.ascender, header.descender};

    return GfntFont(metrics, std::move(fontGlyphs), std::move(bitmapData));
}

}  // namespace robot::io
