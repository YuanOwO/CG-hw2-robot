#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace robot {

// 載入後的 GFNT 點陣字型。與檔案格式版本無關，由 io::GfntParserV1 等 parser 建立。
class GfntFont {
   public:
    struct Metrics {
        int pixelSize;
        int ascender;   // baseline 以上的高度
        int descender;  // baseline 以下的深度，為非負值
    };

    struct Glyph {
        char32_t codepoint;

        int width;
        int height;

        int bearingX;
        int bearingY;

        int advanceX;

        std::size_t bitmapOffset;  // 在 bitmap data 中的位置，每個 pixel 一個 alpha byte
    };

    // glyphs 必須已依 codepoint 嚴格遞增排列，且每個 bitmap 都落在 bitmapData 內；
    // 讀檔時由 parser 負責驗證。
    GfntFont(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> bitmapData);

    const Metrics& metrics() const { return _metrics; }

    const Glyph* findGlyph(char32_t codepoint) const;
    const Glyph* findGlyphOrFallback(char32_t codepoint) const;

    const std::uint8_t* bitmap(const Glyph& glyph) const { return _bitmapData.data() + glyph.bitmapOffset; }

   private:
    Metrics _metrics;
    std::vector<Glyph> _glyphs;
    std::vector<std::uint8_t> _bitmapData;
};

}  // namespace robot
