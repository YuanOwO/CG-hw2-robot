#pragma once

#include <cstdint>

// GFNT 檔案格式定義，只供 parser 使用；程式其他部分透過 GfntFont 取用字型。

namespace robot::io::gfnt {

inline const char MAGIC[4] = {'G', 'F', 'N', 'T'};
inline const uint16_t VERSION = 1;

inline const std::uint64_t HEADER_SIZE = 16;
inline const std::uint64_t GLYPH_RECORD_SIZE = 18;

struct GfntHeaderV1 {
    char magic[4];

    uint16_t version;
    uint16_t pixelSize;

    int16_t ascender;
    int16_t descender;

    uint32_t glyphCount;
};

struct GfntGlyphV1 {
    uint32_t codepoint;

    uint16_t width;
    uint16_t height;

    int16_t bearingX;
    int16_t bearingY;

    uint16_t advanceX;

    uint32_t bitmapOffset;
};

}  // namespace robot::io::gfnt
