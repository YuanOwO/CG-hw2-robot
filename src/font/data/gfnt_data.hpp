#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace robot {

// 載入後的 GFNT 檔案內容：一個字號的點陣字形，單位為像素。
// 與檔案格式版本無關，由 io::GfntParserV1 等 parser 建立。
class GfntData {
   public:
    struct Metrics {
        int pixelSize;
        int ascender;
        int descender;  // 非負值
    };

    struct Glyph {
        char32_t codepoint;

        int width;
        int height;

        int bearingX;
        int bearingY;

        int advanceX;

        std::size_t bitmapOffset;
    };

    // glyphs 必須依 codepoint 嚴格遞增，且每個 bitmap 都在 bitmapData 內；由 parser 驗證。
    GfntData(const Metrics& metrics, std::vector<Glyph> glyphs, std::vector<std::uint8_t> bitmapData);

    const Metrics& metrics() const { return _metrics; }
    const std::vector<Glyph>& glyphs() const { return _glyphs; }

    const Glyph* findGlyph(char32_t codepoint) const;
    const Glyph* findGlyphOrFallback(char32_t codepoint) const;

    // width × height 個 8-bit alpha，第一列在最上面。
    const std::uint8_t* bitmap(const Glyph& glyph) const { return _bitmapData.data() + glyph.bitmapOffset; }

   private:
    Metrics _metrics;
    std::vector<Glyph> _glyphs;
    std::vector<std::uint8_t> _bitmapData;
};

}  // namespace robot
