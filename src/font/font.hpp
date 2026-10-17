#pragma once

#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "font/glut_font.hpp"

namespace robot {

enum class GlyphFont {
    CUBIC11,           // 12 px 像素字型，可整數倍放大
    UNIFONT,           // 16 px 像素字型，可整數倍放大
    NOTO_SANS_CJK_TC,  // 小字號用 GFNT，其他字號由 GOTL 點陣化
};

// 新增字型時要同時加到 enum 與這張表；字型設定檔以這裡的名稱指定字型。
inline constexpr std::pair<GlyphFont, const char*> GLYPH_FONT_NAMES[] = {
    {GlyphFont::CUBIC11,          "CUBIC11"         },
    {GlyphFont::UNIFONT,          "UNIFONT"         },
    {GlyphFont::NOTO_SANS_CJK_TC, "NOTO_SANS_CJK_TC"},
};

const char* glyphFontName(GlyphFont font);
std::optional<GlyphFont> findGlyphFont(const std::string& name);

// 只保存字型與字號，可以寫成全域常數；實際的 SizedFont 在使用時才向 fonts() 取得。
struct GlyphFontStyle {
    GlyphFont font;
    int pixelSize;

    bool operator==(const GlyphFontStyle& other) const { return font == other.font && pixelSize == other.pixelSize; }
    bool operator!=(const GlyphFontStyle& other) const { return !(*this == other); }
};

// 文字一律優先使用 GlyphFontStyle；GLUT 字型只支援 ASCII，保留給相容用途。
using FontStyle = std::variant<GlyphFontStyle, BitmapFontStyle, StrokeFontStyle>;

float getFontWidth(const GlyphFontStyle& style, const std::string& text);
float getFontHeight(const GlyphFontStyle& style, const std::string& text);

inline float getFontWidth(const FontStyle& style, const std::string& text) {
    return std::visit([&text](const auto& s) { return getFontWidth(s, text); }, style);
}

inline float getFontHeight(const FontStyle& style, const std::string& text) {
    return std::visit([&text](const auto& s) { return getFontHeight(s, text); }, style);
}

}  // namespace robot
