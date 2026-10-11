#include "font/glut_font.hpp"

#include <algorithm>
#include <stdexcept>

#include "font/utils.hpp"
#include "platform/glut.hpp"

namespace robot {

#pragma region Font mapping

void* mapFont(const BitmapFont& font) {
    switch (font) {
    case BitmapFont::FIXED_8_BY_13:
        return GLUT_BITMAP_8_BY_13;
    case BitmapFont::FIXED_9_BY_15:
        return GLUT_BITMAP_9_BY_15;
    case BitmapFont::HELVETICA_10:
        return GLUT_BITMAP_HELVETICA_10;
    case BitmapFont::HELVETICA_12:
        return GLUT_BITMAP_HELVETICA_12;
    case BitmapFont::HELVETICA_18:
        return GLUT_BITMAP_HELVETICA_18;
    case BitmapFont::TIMES_ROMAN_10:
        return GLUT_BITMAP_TIMES_ROMAN_10;
    case BitmapFont::TIMES_ROMAN_24:
        return GLUT_BITMAP_TIMES_ROMAN_24;
    }

    throw std::invalid_argument("Unknown bitmap font");
}

void* mapFont(const StrokeFont& font) {
    switch (font) {
    case StrokeFont::ROMAN:
        return GLUT_STROKE_ROMAN;
    case StrokeFont::MONO_ROMAN:
        return GLUT_STROKE_MONO_ROMAN;
    }

    throw std::invalid_argument("Unknown stroke font");
}

#pragma endregion  // Font mapping

#pragma region Font metrics

float getLineHeight(const BitmapFont& font) {
    switch (font) {
    case BitmapFont::FIXED_8_BY_13:
    case BitmapFont::HELVETICA_10:
    case BitmapFont::TIMES_ROMAN_10:
        return 14.0f;

    case BitmapFont::FIXED_9_BY_15:
    case BitmapFont::HELVETICA_12:
        return 16.0f;

    case BitmapFont::HELVETICA_18:
        return 23.0f;

    case BitmapFont::TIMES_ROMAN_24:
        return 29.0f;
    }

    throw std::invalid_argument("Unknown bitmap font");
}

float getOriginY(const BitmapFont& font) {
    switch (font) {
    case BitmapFont::FIXED_8_BY_13:
    case BitmapFont::HELVETICA_10:
        return 3.0f;

    case BitmapFont::FIXED_9_BY_15:
    case BitmapFont::HELVETICA_12:
    case BitmapFont::TIMES_ROMAN_10:
        return 4.0f;

    case BitmapFont::HELVETICA_18:
        return 5.0f;

    case BitmapFont::TIMES_ROMAN_24:
        return 7.0f;
    }

    throw std::invalid_argument("Unknown bitmap font");
}

float getLineHeight(const StrokeFont& font) {
    switch (font) {
    case StrokeFont::ROMAN:
    case StrokeFont::MONO_ROMAN:
        return 152.381f;
    }

    throw std::invalid_argument("Unknown stroke font");
}

float getAscent(const StrokeFont& font) {
    switch (font) {
    case StrokeFont::ROMAN:
    case StrokeFont::MONO_ROMAN:
        return 119.048f;
    }

    throw std::invalid_argument("Unknown stroke font");
}

#pragma endregion  // Font metrics

#pragma region Text size

float getFontWidth(const BitmapFontStyle& style, const std::string& text) {
    void* glutFont = mapFont(style.font);

    // 與繪製相同：每個位元組以 unsigned char 交給 GLUT，非 ASCII 位元組畫出的是 Latin-1 字元。
    int width = 0, maxWidth = 0;
    for (const char ch : text) {
        const auto c = static_cast<unsigned char>(ch);

        if (c == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            continue;
        }
        width += glutBitmapWidth(glutFont, c);
    }

    return static_cast<float>(std::max(maxWidth, width));
}

float getFontWidth(const StrokeFontStyle& style, const std::string& text) {
    void* glutFont = mapFont(style.font);

    // 與繪製相同：stroke 字型只有 ASCII，其餘位元組不畫也不佔寬度。
    int width = 0, maxWidth = 0;
    for (const char ch : text) {
        const auto c = static_cast<unsigned char>(ch);

        if (c == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            continue;
        }
        if (c >= 128) {
            continue;
        }
        width += glutStrokeWidth(glutFont, c);
    }

    return static_cast<float>(std::max(maxWidth, width)) * style.scale;
}

float getFontHeight(const BitmapFontStyle& style, const std::string& text) {
    return static_cast<float>(countLines(text)) * getLineHeight(style.font);
}

float getFontHeight(const StrokeFontStyle& style, const std::string& text) {
    return static_cast<float>(countLines(text)) * getLineHeight(style.font) * style.scale;
}

#pragma endregion  // Text size

}  // namespace robot
