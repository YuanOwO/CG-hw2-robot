#pragma once

#include <optional>

#include "font/data/gfnt_data.hpp"
#include "font/sized_font.hpp"

namespace robot {

// 直接使用 GFNT 事先點陣化的字形。像素字型可以整數倍放大，放大後每個像素仍是銳利的方塊。
class GfntFont : public SizedFont {
   public:
    // data 必須比這個物件活得久；由 FontLibrary 保證。
    GfntFont(const GfntData& data, int scale);

    FontMetrics metrics() const override;
    std::optional<Glyph> glyph(char32_t codepoint) const override;

   private:
    const GfntData& _data;
    int _scale;
};

}  // namespace robot
