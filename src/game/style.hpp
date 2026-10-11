#pragma once

#include "common/color.hpp"
#include "font/font.hpp"
#include "gui/button.hpp"

namespace robot::game::style {

inline const FontStyle TitleFont = GlyphFontStyle{GlyphFont::NOTO_SANS_CJK_TC, 72};
inline const FontStyle HeadingFont = GlyphFontStyle{GlyphFont::NOTO_SANS_CJK_TC, 32};
inline const FontStyle BodyFont = GlyphFontStyle{GlyphFont::UNIFONT, 16};

inline const ColorRGBA Text{0.93f, 0.95f, 0.98f};
inline const ColorRGBA MutedText{0.62f, 0.67f, 0.75f};

inline const ColorRGBA MenuBackground{0.11f, 0.13f, 0.17f};

// 疊在世界上：世界仍看得到，但不會搶走選單的注意力
inline const ColorRGBA PauseOverlay{0.0f, 0.0f, 0.0f, 0.6f};

inline const gui::ButtonStyle MenuButton{
    BodyFont,
    Text,
    ColorRGBA{0.21f, 0.24f, 0.30f}, // fill
    ColorRGBA{0.29f, 0.33f, 0.41f}, // hovered
    ColorRGBA{0.16f, 0.18f, 0.23f}, // pressed
    ColorRGBA{0.40f, 0.45f, 0.54f}, // border
    ColorRGBA{0.38f, 0.60f, 1.00f}, // focusBorder
};

}  // namespace robot::game::style
