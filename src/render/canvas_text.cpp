#include <string>
#include <variant>

#include "common/point.hpp"
#include "font/font_library.hpp"
#include "platform/glut.hpp"
#include "render/canvas.hpp"

namespace robot::render {

namespace {

void renderBitmap(const std::string& text, const BitmapFontStyle& style, float left, float top) {
    void* font = mapFont(style.font);

    const float lineHeight = getLineHeight(style.font);

    float x = left;
    float y = top + lineHeight - getOriginY(style.font);

    for (const char ch : text) {
        const auto c = static_cast<unsigned char>(ch);

        if (c == '\n') {
            x = left;
            y += lineHeight;  // 元素座標向下為正。
            continue;
        }

        glRasterPos2f(x, y);
        glutBitmapCharacter(font, c);

        x += static_cast<float>(glutBitmapWidth(font, c));
    }
}

void renderStroke(Canvas& canvas, const std::string& text, const StrokeFontStyle& style, float left,
                  float top) {
    void* font = mapFont(style.font);

    const float scale = style.scale;
    const float lineHeight = getLineHeight(style.font) * scale;

    float x = left;
    float y = top + getAscent(style.font) * scale;

    for (const char ch : text) {
        const auto c = static_cast<unsigned char>(ch);

        if (c == '\n') {
            x = left;
            y += lineHeight;
            continue;
        }

        // Stroke 字型僅處理 ASCII。
        if (c >= 128) {
            continue;
        }

        canvas.pushTransform();
        canvas.translate(x, y);

        // Stroke 字型原本向上為正。
        // 翻轉 Y，配合目前 UI 向下為正的座標系。
        glScalef(scale, -scale, 1.0f);

        // 此函式會移動目前矩陣，因此每個字都恢復矩陣。
        glutStrokeCharacter(font, c);

        canvas.popTransform();

        // 與目前 getFontWidth() 使用相同的寬度計算。
        x += static_cast<float>(glutStrokeWidth(font, c)) * scale;
    }
}

void renderGlyphs(const std::string& text, const GlyphFontStyle& style, float left, float top) {
    const TextLayout layout = fonts().get(style).layout(text);

    // glDrawPixels 不套用 glColor。以 GL_ALPHA 格式送出時 RGB 視為 0，
    // 因此用 pixel transfer 的 bias 填入文字顏色、scale 乘上文字透明度，直接畫字形的 alpha bitmap。
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    GLint previousUnpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glPixelTransferf(GL_RED_BIAS, currentColor[0]);
    glPixelTransferf(GL_GREEN_BIAS, currentColor[1]);
    glPixelTransferf(GL_BLUE_BIAS, currentColor[2]);
    glPixelTransferf(GL_ALPHA_SCALE, currentColor[3]);

    for (const PlacedGlyph& placed : layout.glyphs) {
        const Glyph& glyph = placed.glyph;

        // bitmap 的第一列是頂端；負的 Y zoom 讓 OpenGL 由上往下畫，像素字型放大時每個像素畫成 scale × scale。
        const float scale = static_cast<float>(glyph.scale);
        glPixelZoom(scale, -scale);
        glRasterPos2f(left + placed.x, top + placed.y);
        glDrawPixels(glyph.bitmapWidth, glyph.bitmapHeight, GL_ALPHA, GL_UNSIGNED_BYTE, glyph.alpha);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
    glPopAttrib();
}

}  // namespace

void Canvas::drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color,
                             const Point& origin) {
    if (text.empty()) {
        return;
    }

    // 避免影響其他元素的顏色、raster position 與線寬。
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glColor4f(color.r, color.g, color.b, color.a);

    if (const auto* bitmap = std::get_if<BitmapFontStyle>(&fontStyle)) {
        renderBitmap(text, *bitmap, origin.x, origin.y);
    } else if (const auto* stroke = std::get_if<StrokeFontStyle>(&fontStyle)) {
        glLineWidth(1.0f);
        renderStroke(*this, text, *stroke, origin.x, origin.y);
    } else if (const auto* glyphs = std::get_if<GlyphFontStyle>(&fontStyle)) {
        renderGlyphs(text, *glyphs, origin.x, origin.y);
    }

    glPopAttrib();
}

}  // namespace robot::render
