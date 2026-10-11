#pragma once

#include <cstdint>

// GFNT v1 檔案格式，完整說明見同資料夾的 gfnt-v1.md。
// 只供 parser 使用；程式其他部分透過 GfntFont 取用字型。

namespace robot::io::gfnt {

inline const char MAGIC[4] = {'G', 'F', 'N', 'T'};
inline const std::uint16_t VERSION = 1;

inline const std::uint64_t HEADER_SIZE = 16;
inline const std::uint64_t GLYPH_RECORD_SIZE = 18;

// 只存放解碼後的值，parser 逐欄讀取而不是直接 memcpy，因此不需要 #pragma pack。
struct GfntHeaderV1 {
    char magic[4];

    std::uint16_t version;
    std::uint16_t pixelSize;

    std::int16_t ascender;
    std::int16_t descender;

    std::uint32_t glyphCount;
};

struct GfntGlyphV1 {
    std::uint32_t codepoint;

    std::uint16_t width;
    std::uint16_t height;

    std::int16_t bearingX;
    std::int16_t bearingY;

    std::uint16_t advanceX;

    std::uint32_t bitmapOffset;
};

}  // namespace robot::io::gfnt
