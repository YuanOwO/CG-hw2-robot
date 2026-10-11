#pragma once

#include <cstdint>

// GOTL 檔案格式定義，只供 parser 使用；程式其他部分透過 GotlFont 取用字型。

namespace robot::io::gotl {

inline const char MAGIC[4] = {'G', 'O', 'T', 'L'};
inline const uint16_t VERSION = 1;

inline const std::uint64_t HEADER_SIZE = 32;
inline const std::uint64_t GLYPH_RECORD_SIZE = 20;

struct GotlHeaderV1 {
    char magic[4];

    uint16_t version;
    uint16_t unitsPerEm;

    int16_t ascender;
    int16_t descender;

    uint8_t curveOrder;
    uint8_t reserved;
    uint16_t flags;

    uint32_t glyphCount;
    uint32_t glyphTableOffset;
    uint32_t outlineDataOffset;
    uint32_t outlineDataSize;
};

struct GotlGlyphV1 {
    uint32_t codepoint;
    uint32_t outlineOffset;

    uint16_t opCount;
    uint16_t advanceX;

    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
};

}  // namespace robot::io::gotl
