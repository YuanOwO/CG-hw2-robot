#pragma once

#include <cstdint>

// GOTL v1 檔案格式，完整說明見同資料夾的 gotl-v1.md。
// 只供 parser 使用；程式其他部分透過 GotlData 取用字型。

namespace robot::io::gotl {

inline const char MAGIC[4] = {'G', 'O', 'T', 'L'};
inline const std::uint16_t VERSION = 1;

inline const std::uint64_t HEADER_SIZE = 32;
inline const std::uint64_t GLYPH_RECORD_SIZE = 20;

// 只存放解碼後的值，parser 逐欄讀取而不是直接 memcpy，因此不需要 #pragma pack。
struct GotlHeaderV1 {
    char magic[4];

    std::uint16_t version;
    std::uint16_t unitsPerEm;

    std::int16_t ascender;
    std::int16_t descender;

    std::uint8_t curveOrder;
    std::uint8_t reserved;
    std::uint16_t flags;

    std::uint32_t glyphCount;
    std::uint32_t glyphTableOffset;
    std::uint32_t outlineDataOffset;
    std::uint32_t outlineDataSize;
};

struct GotlGlyphV1 {
    std::uint32_t codepoint;
    std::uint32_t outlineOffset;

    std::uint16_t opCount;
    std::uint16_t advanceX;

    std::int16_t xMin;
    std::int16_t yMin;
    std::int16_t xMax;
    std::int16_t yMax;
};

}  // namespace robot::io::gotl
