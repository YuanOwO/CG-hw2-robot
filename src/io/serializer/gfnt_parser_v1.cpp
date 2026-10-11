#include "io/serializer/gfnt_parser_v1.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "io/serializer/gfnt.hpp"

namespace robot::io {

namespace {

// 所有欄位都用這個 helper 讀，這樣遇到截斷的檔案時，
// 不會把只讀到一半的資料當成有效字型繼續處理。
void readExact(std::istream& input, void* destination, std::size_t size) {
    input.read(static_cast<char*>(destination), static_cast<std::streamsize>(size));

    if (!input) {
        throw std::runtime_error("Unexpected end of GFNT v1 file");
    }
}

// GFNT 在檔案中固定使用 little-endian。這裡手動組合 bytes，
// 避免 loader 暗中依賴執行平台的 endian。
std::uint16_t readU16(std::istream& input) {
    std::array<std::uint8_t, 2> bytes{};
    readExact(input, bytes.data(), bytes.size());

    return static_cast<std::uint16_t>(bytes[0]) | (static_cast<std::uint16_t>(bytes[1]) << 8);
}

std::int16_t readI16(std::istream& input) {
    const std::uint16_t value = readU16(input);

    if (value <= static_cast<std::uint16_t>(std::numeric_limits<std::int16_t>::max())) {
        return static_cast<std::int16_t>(value);
    }

    // 用明確的運算還原 two's-complement，不依賴 unsigned 轉 signed
    // 超出範圍時的 implementation-defined 行為。
    return static_cast<std::int16_t>(static_cast<std::int32_t>(value) - 0x10000);
}

std::uint32_t readU32(std::istream& input) {
    std::array<std::uint8_t, 4> bytes{};
    readExact(input, bytes.data(), bytes.size());

    return static_cast<std::uint32_t>(bytes[0]) | (static_cast<std::uint32_t>(bytes[1]) << 8) |
           (static_cast<std::uint32_t>(bytes[2]) << 16) | (static_cast<std::uint32_t>(bytes[3]) << 24);
}

gfnt::GfntHeaderV1 readHeader(std::istream& input) {
    gfnt::GfntHeaderV1 header{};

    readExact(input, header.magic, sizeof(header.magic));
    header.version = readU16(input);
    header.pixelSize = readU16(input);
    header.ascender = readI16(input);
    header.descender = readI16(input);
    header.glyphCount = readU32(input);

    return header;
}

gfnt::GfntGlyphV1 readGlyph(std::istream& input) {
    gfnt::GfntGlyphV1 glyph{};

    glyph.codepoint = readU32(input);
    glyph.width = readU16(input);
    glyph.height = readU16(input);
    glyph.bearingX = readI16(input);
    glyph.bearingY = readI16(input);
    glyph.advanceX = readU16(input);
    glyph.bitmapOffset = readU32(input);

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
    // 先取得剩餘資料大小，之後才能在分配 glyph table 前確認
    // header 裡的 glyphCount 沒有指向資料外。
    const std::streampos start = _input.tellg();
    _input.seekg(0, std::ios::end);
    const std::streampos end = _input.tellg();
    _input.seekg(start);

    if (start < 0 || end < 0 || !_input) {
        throw std::runtime_error("GFNT v1 input must be seekable");
    }

    const std::streamoff fileSize = end - start;

    if (fileSize < static_cast<std::streamoff>(sizeof(gfnt::GfntHeaderV1))) {
        throw std::runtime_error("GFNT file is too small");
    }

    const gfnt::GfntHeaderV1 header = readHeader(_input);

    if (std::memcmp(header.magic, gfnt::MAGIC, sizeof(gfnt::MAGIC)) != 0) {
        throw std::runtime_error("Invalid GFNT magic");
    }

    if (header.version != gfnt::VERSION) {
        throw std::runtime_error("Unsupported GFNT version: " + std::to_string(header.version));
    }

    if (header.pixelSize == 0) {
        throw std::runtime_error("GFNT pixelSize must not be zero");
    }

    if (header.ascender < 0 || header.descender < 0) {
        throw std::runtime_error("GFNT ascender and descender must be non-negative");
    }

    const std::uint64_t tableSize = static_cast<std::uint64_t>(header.glyphCount) * sizeof(gfnt::GfntGlyphV1);

    // Bitmap Data 緊接在 Glyph Table 後面，所以不需要額外的
    // section offset 欄位。
    const std::uint64_t bitmapDataOffset = sizeof(gfnt::GfntHeaderV1) + tableSize;

    if (bitmapDataOffset > static_cast<std::uint64_t>(fileSize)) {
        throw std::runtime_error("GFNT glyph table extends past end of file");
    }

    std::vector<gfnt::GfntGlyphV1> glyphs;
    glyphs.reserve(header.glyphCount);

    for (std::uint32_t i = 0; i < header.glyphCount; i++) {
        glyphs.push_back(readGlyph(_input));
    }

    const std::size_t bitmapDataSize =
        static_cast<std::size_t>(static_cast<std::uint64_t>(fileSize) - bitmapDataOffset);

    std::vector<std::uint8_t> bitmapData(bitmapDataSize);

    if (bitmapDataSize != 0) {
        readExact(_input, bitmapData.data(), bitmapData.size());
    }

    // 載入時一次驗證完整個字型，渲染時就可以直接取用 bitmap，
    // 不必每畫一個字都重複做邊界檢查。
    for (std::size_t i = 0; i < glyphs.size(); i++) {
        const auto& glyph = glyphs[i];

        // findGlyph() 使用 binary search，因此這裡不只要檢查排序，
        // 也要拒絕重複的 codepoint。
        if (i != 0 && glyphs[i - 1].codepoint >= glyph.codepoint) {
            throw std::runtime_error("GFNT glyph table is not strictly sorted");
        }

        // Unicode code point 的最大合法值是 U+10FFFF，
        // U+D800 至 U+DFFF 為 UTF-16 surrogate，不是合法字元。
        if (glyph.codepoint > 0x10FFFF || (glyph.codepoint >= 0xD800 && glyph.codepoint <= 0xDFFF)) {
            throw std::runtime_error("GFNT contains an invalid Unicode code point");
        }

        const std::size_t bitmapOffset = glyph.bitmapOffset;
        const std::size_t bitmapSize = static_cast<std::size_t>(glyph.width) * static_cast<std::size_t>(glyph.height);

        // 用減法比較可以避免 bitmapOffset + bitmapSize 溢位。
        if (bitmapOffset > bitmapData.size() || bitmapSize > bitmapData.size() - bitmapOffset) {
            throw std::runtime_error("GFNT glyph bitmap extends past Bitmap Data");
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
