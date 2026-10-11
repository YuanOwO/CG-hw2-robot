#pragma once

#include <cstdint>
#include <vector>

#include "font/data/gotl_data.hpp"

namespace robot {

// GOTL 字形點陣化後的結果，單位為像素，欄位意義與 GFNT 的字形相同。
struct RasterizedGlyph {
    int width;
    int height;

    int bearingX;
    int bearingY;

    int advanceX;

    std::vector<std::uint8_t> bitmap;  // width × height 個 alpha byte，row-major
};

// 以有號面積累加法點陣化字形，以 nonzero 規則填色並反鋸齒。
RasterizedGlyph rasterizeGlyph(const GotlData& font, const GotlData::Glyph& glyph, int pixelSize);

}  // namespace robot
