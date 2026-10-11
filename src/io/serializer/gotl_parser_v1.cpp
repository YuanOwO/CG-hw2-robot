#include "io/serializer/gotl_parser_v1.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/utf8.hpp"
#include "io/serializer/byte_reader.hpp"

namespace robot::io {

namespace {

gotl::GotlHeaderV1 readHeader(serializer::ByteReader& reader) {
    gotl::GotlHeaderV1 header{};

    reader.read(header.magic, sizeof(header.magic));
    header.version = reader.u16();
    header.unitsPerEm = reader.u16();
    header.ascender = reader.i16();
    header.descender = reader.i16();
    header.curveOrder = reader.u8();
    header.reserved = reader.u8();
    header.flags = reader.u16();
    header.glyphCount = reader.u32();
    header.glyphTableOffset = reader.u32();
    header.outlineDataOffset = reader.u32();
    header.outlineDataSize = reader.u32();

    return header;
}

gotl::GotlGlyphV1 readGlyph(serializer::ByteReader& reader) {
    gotl::GotlGlyphV1 glyph{};

    glyph.codepoint = reader.u32();
    glyph.outlineOffset = reader.u32();
    glyph.opCount = reader.u16();
    glyph.advanceX = reader.u16();
    glyph.xMin = reader.i16();
    glyph.yMin = reader.i16();
    glyph.xMax = reader.i16();
    glyph.yMax = reader.i16();

    return glyph;
}

GotlFont::Glyph toGlyph(const gotl::GotlGlyphV1& glyph) {
    return GotlFont::Glyph{
        static_cast<char32_t>(glyph.codepoint),
        glyph.outlineOffset,
        glyph.opCount,
        glyph.advanceX,
        glyph.xMin,
        glyph.yMin,
        glyph.xMax,
        glyph.yMax,
    };
}

}  // namespace

struct GotlParserV1::BoundingBox {
    int xMin, yMin, xMax, yMax;

    bool operator==(const BoundingBox& other) const {
        return xMin == other.xMin && yMin == other.yMin && xMax == other.xMax && yMax == other.yMax;
    }
};

GotlParserV1::BoundingBox GotlParserV1::validateOutline(const std::vector<std::uint8_t>& outlineData,
                                                        const gotl::GotlGlyphV1& glyph, int curveOrder) const {
    // 錯誤訊息標出是哪個字元；只在拋出例外時才組字串。
    const auto where = [&glyph] { return " (" + utf8::formatCodepoint(glyph.codepoint) + ")"; };

    const std::uint64_t outlineDataSize = outlineData.size();
    const std::uint64_t offset = glyph.outlineOffset;
    const std::uint64_t opCount = glyph.opCount;
    const std::uint64_t opBytes = opCount + (opCount & 1);

    if (offset % 2 != 0) {
        throw error("outlineOffset must be even" + where());
    }

    // 先確認 offset 在範圍內，之後的減法才不會繞回。
    if (offset > outlineDataSize || opBytes > outlineDataSize - offset) {
        throw error("outline ops extend past Outline Data" + where());
    }

    const std::uint8_t* ops = outlineData.data() + offset;

    if (ops[0] != 0) {
        throw error("outline must start with MoveTo" + where());
    }

    std::uint64_t pointCount = 0;
    for (std::uint64_t i = 0; i < opCount; i++) {
        switch (ops[i]) {
        case 0:
        case 1:
            pointCount += 1;
            break;
        case 2:
            pointCount += static_cast<std::uint64_t>(curveOrder);
            break;
        default:
            throw error("outline contains an invalid op " + std::to_string(ops[i]) + where());
        }
    }

    if (4 * pointCount > outlineDataSize - offset - opBytes) {
        throw error("outline points extend past Outline Data" + where());
    }

    serializer::ByteReader points(ops + opBytes, static_cast<std::size_t>(4 * pointCount));

    const int firstX = points.i16();
    const int firstY = points.i16();
    BoundingBox box{firstX, firstY, firstX, firstY};

    for (std::uint64_t i = 1; i < pointCount; i++) {
        const int x = points.i16();
        const int y = points.i16();

        box.xMin = std::min(box.xMin, x);
        box.yMin = std::min(box.yMin, y);
        box.xMax = std::max(box.xMax, x);
        box.yMax = std::max(box.yMax, y);
    }

    return box;
}

GotlFont GotlParserV1::parse() {
    const std::uint64_t fileSize = inputSize();

    // 條件 1：檔案長度、magic、version
    if (fileSize < gotl::HEADER_SIZE) {
        throw error("file is too small");
    }

    const std::vector<std::uint8_t> headerBytes = readSection(0, gotl::HEADER_SIZE);
    serializer::ByteReader headerReader(headerBytes);
    const gotl::GotlHeaderV1 header = readHeader(headerReader);

    if (std::memcmp(header.magic, gotl::MAGIC, sizeof(gotl::MAGIC)) != 0) {
        throw error("magic must be \"GOTL\"");
    }

    if (header.version != gotl::VERSION) {
        throw error("unsupported version " + std::to_string(header.version));
    }

    // 條件 2：header 欄位
    if (header.unitsPerEm == 0) {
        throw error("unitsPerEm must not be zero");
    }

    if (header.ascender < 0 || header.descender < 0) {
        throw error("ascender and descender must be non-negative");
    }

    if (header.curveOrder != 2 && header.curveOrder != 3) {
        throw error("curveOrder must be 2 or 3, got " + std::to_string(header.curveOrder));
    }

    if (header.reserved != 0) {
        throw error("reserved byte must be zero");
    }

    // 不認得的 flags 代表不同的編碼方式，不能以 v1 的結構解析。
    if (header.flags != 0) {
        throw error("flags must be zero, got " + std::to_string(header.flags));
    }

    // 條件 3：區段位移
    const std::uint64_t tableOffset = header.glyphTableOffset;
    const std::uint64_t outlineOffset = header.outlineDataOffset;
    const std::uint64_t outlineSize = header.outlineDataSize;

    if (tableOffset % 4 != 0 || tableOffset < gotl::HEADER_SIZE) {
        throw error("glyphTableOffset must be a multiple of 4 and at least 32");
    }

    if (outlineOffset % 4 != 0 || outlineOffset < gotl::HEADER_SIZE) {
        throw error("outlineDataOffset must be a multiple of 4 and at least 32");
    }

    // 條件 4：區段範圍。全部以 64-bit 計算，32-bit 欄位相加或相乘都不會溢位。
    const std::uint64_t tableSize = static_cast<std::uint64_t>(header.glyphCount) * gotl::GLYPH_RECORD_SIZE;

    if (tableOffset + tableSize > fileSize) {
        throw error("glyph table extends past end of file");
    }

    if (outlineOffset + outlineSize > fileSize) {
        throw error("outline data extends past end of file");
    }

    const bool sectionsOverlap = tableSize != 0 && outlineSize != 0 && tableOffset < outlineOffset + outlineSize &&
                                 outlineOffset < tableOffset + tableSize;
    if (sectionsOverlap) {
        throw error("glyph table and outline data overlap");
    }

    // 已確認 glyphCount 不超出檔案，才依它配置記憶體。兩個區段各自讀取，
    // outline data 讀進來後直接交給 GotlFont，不必先讀整份檔案再複製。
    const std::vector<std::uint8_t> tableBytes = readSection(tableOffset, tableSize);
    std::vector<std::uint8_t> outlineData = readSection(outlineOffset, outlineSize);

    serializer::ByteReader tableReader(tableBytes);

    std::vector<gotl::GotlGlyphV1> glyphs;
    glyphs.reserve(header.glyphCount);

    for (std::uint32_t i = 0; i < header.glyphCount; i++) {
        glyphs.push_back(readGlyph(tableReader));
    }

    // 多筆記錄可以共用同一個輪廓，只需驗證一次；key 為 (outlineOffset, opCount)。
    std::unordered_map<std::uint64_t, BoundingBox> validatedOutlines;

    for (std::size_t i = 0; i < glyphs.size(); i++) {
        const auto& glyph = glyphs[i];
        const auto where = [&glyph] { return " (" + utf8::formatCodepoint(glyph.codepoint) + ")"; };

        // 條件 5：findGlyph() 使用二分搜尋，必須嚴格遞增（不可重複）。
        if (i != 0 && glyphs[i - 1].codepoint >= glyph.codepoint) {
            throw error("glyph table is not strictly sorted" + where());
        }

        // 條件 6：合法的 Unicode scalar value。
        if (!utf8::isScalarValue(glyph.codepoint)) {
            throw error("invalid Unicode code point" + where());
        }

        const BoundingBox recordBox{glyph.xMin, glyph.yMin, glyph.xMax, glyph.yMax};

        // 條件 7：沒有輪廓的字元。
        if (glyph.opCount == 0) {
            if (glyph.outlineOffset > outlineSize) {
                throw error("outlineOffset extends past Outline Data" + where());
            }

            if (!(recordBox == BoundingBox{0, 0, 0, 0})) {
                throw error("glyph without outline must have an empty bounding box" + where());
            }

            continue;
        }

        // 條件 8：有輪廓的字元。
        const std::uint64_t key = (static_cast<std::uint64_t>(glyph.outlineOffset) << 16) | glyph.opCount;

        auto it = validatedOutlines.find(key);
        if (it == validatedOutlines.end()) {
            const BoundingBox actualBox = validateOutline(outlineData, glyph, header.curveOrder);
            it = validatedOutlines.emplace(key, actualBox).first;
        }

        // bbox 必須與實際的點完全相等，不可放寬。
        if (!(recordBox == it->second)) {
            throw error("glyph bounding box does not match its outline points" + where());
        }
    }

    std::vector<GotlFont::Glyph> fontGlyphs;
    fontGlyphs.reserve(glyphs.size());

    for (const auto& glyph : glyphs) {
        fontGlyphs.push_back(toGlyph(glyph));
    }

    const GotlFont::Metrics metrics{header.unitsPerEm, header.ascender, header.descender, header.curveOrder};

    return GotlFont(metrics, std::move(fontGlyphs), std::move(outlineData));
}

}  // namespace robot::io
