#pragma once

#include <optional>
#include <unordered_map>

#include "font/data/gotl_data.hpp"
#include "font/raster/outline_rasterizer.hpp"
#include "font/sized_font.hpp"

namespace robot {

// 將 GOTL 輪廓點陣化成指定字號。每個字只在第一次用到時點陣化，結果留在快取中。
class GotlFont : public SizedFont {
   public:
    // data 必須比這個物件活得久；由 FontLibrary 保證。
    GotlFont(const GotlData& data, int pixelSize);

    FontMetrics metrics() const override;
    std::optional<Glyph> glyph(char32_t codepoint) const override;

   private:
    const GotlData& _data;
    int _pixelSize;

    // 快取不影響對外的結果，因此 glyph() 仍是 const。key 是實際使用的字形，
    // 不同缺字都替代成同一個字形時只點陣化一次。
    // unordered_map 新增元素時不會搬動既有元素，回傳給呼叫端的 bitmap 指標保持有效。
    mutable std::unordered_map<char32_t, RasterizedGlyph> _cache;
};

}  // namespace robot
