#pragma once

#include <string>
#include <variant>

namespace robot {

// GLUT 內建字型，只支援 ASCII。

enum class BitmapFont {
    FIXED_8_BY_13,
    FIXED_9_BY_15,
    HELVETICA_10,
    HELVETICA_12,
    HELVETICA_18,
    TIMES_ROMAN_10,
    TIMES_ROMAN_24,
};

enum class StrokeFont {
    ROMAN,
    MONO_ROMAN,
};

using GlutFont = std::variant<BitmapFont, StrokeFont>;

struct BitmapFontStyle {
    BitmapFont font;

    bool operator==(const BitmapFontStyle& other) const { return font == other.font; }
    bool operator!=(const BitmapFontStyle& other) const { return !(*this == other); }
};

struct StrokeFontStyle {
    StrokeFont font;
    float scale = 1.0f;  // stroke 字型以字型座標繪製，scale 為 1 時行高約 152 px

    bool operator==(const StrokeFontStyle& other) const { return font == other.font && scale == other.scale; }
    bool operator!=(const StrokeFontStyle& other) const { return !(*this == other); }
};

// 回傳 GLUT 的字型 handle，供 glutBitmapCharacter / glutStrokeCharacter 使用。
void* mapFont(const BitmapFont& font);
void* mapFont(const StrokeFont& font);

inline void* mapFont(const GlutFont& font) {
    return std::visit([](const auto& f) { return mapFont(f); }, font);
}

// Apple GLUT 沒有 glutBitmapHeight / glutStrokeHeight，度量改為查表，數值與 FreeGLUT 的字型資料相同。

float getLineHeight(const BitmapFont& font);  // 像素
float getOriginY(const BitmapFont& font);     // 基線位置 = 行頂端 + 行高 − originY

float getLineHeight(const StrokeFont& font);  // 字型座標，需乘上 StrokeFontStyle::scale
float getAscent(const StrokeFont& font);

float getFontWidth(const BitmapFontStyle& style, const std::string& text);
float getFontWidth(const StrokeFontStyle& style, const std::string& text);

float getFontHeight(const BitmapFontStyle& style, const std::string& text);
float getFontHeight(const StrokeFontStyle& style, const std::string& text);

}  // namespace robot
