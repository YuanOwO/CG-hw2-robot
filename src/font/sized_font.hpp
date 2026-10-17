#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace robot {

// 以下皆為畫面像素。

struct FontMetrics {
    int pixelSize;
    int ascender;
    int descender;  // 非負值

    int lineHeight() const { return ascender + descender; }
};

// 一個字形在畫面上的大小與位置，以及繪製用的 bitmap。
// 像素字型整數倍放大時，bitmap 仍是原始大小，繪製時每個像素放大 scale 倍。
struct Glyph {
    int width;  // = bitmapWidth × scale
    int height;

    int bearingX;  // pen 到 bitmap 左緣，向右為正
    int bearingY;  // 基線到 bitmap 頂端，向上為正

    int advanceX;

    int bitmapWidth;
    int bitmapHeight;
    int scale;
    const std::uint8_t* alpha;  // bitmapWidth × bitmapHeight 個 8-bit alpha，第一列在最上面
};

struct PlacedGlyph {
    float x;  // bitmap 左上角，相對於文字框左上角
    float y;
    Glyph glyph;
};

struct TextLayout {
    float width;
    float height;
    std::vector<PlacedGlyph> glyphs;  // 只包含有墨跡的字形，空白等字元只推進 pen
};

// 某個字型的某一個字號。字形來源（GFNT 或 GOTL）由子類別決定，排版與量測共用同一份實作，
// 量出來的大小與畫出來的位置因此一定一致。
class SizedFont {
   public:
    virtual ~SizedFont() = default;

    virtual FontMetrics metrics() const = 0;

    // 缺字時已依序以 U+FFFD、'?' 替代；都沒有時回傳 std::nullopt。
    virtual std::optional<Glyph> glyph(char32_t codepoint) const = 0;

    // 以 '\n' 換行；連替代字元都沒有的字以一個字號寬推進 pen，避免後面的字疊在一起。
    TextLayout layout(const std::string& text) const;

    float measureWidth(const std::string& text) const;
    float measureHeight(const std::string& text) const;
};

}  // namespace robot
