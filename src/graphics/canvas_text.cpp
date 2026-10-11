#include <cmath>
#include <string>
#include <variant>

#include "common/point.hpp"
#include "font/font_library.hpp"
#include "platform/glut.hpp"
#include "graphics/canvas.hpp"

namespace robot::graphics {

namespace {

#pragma region Alignment

float alignOffset(TextAlign align, float size) {
    switch (align) {
    case TextAlign::START:
        return 0.0f;
    case TextAlign::CENTER:
        return size / 2.0f;
    case TextAlign::END:
        return size;
    }
    return 0.0f;
}

// 文字框的左上角。取整後，字形的位移也都是整數像素，每個像素都對齊螢幕像素。
Point alignedOrigin(const Point& position, TextAnchor anchor, float width, float height) {
    return Point(std::round(position.x - alignOffset(anchor.horizontal, width)),
                 std::round(position.y - alignOffset(anchor.vertical, height)));
}

#pragma endregion  // Alignment

#pragma region Raster position

// glDrawPixels 與 glutBitmapCharacter 從 raster position 開始畫。直接用 glRasterPos 指定位置時，
// 只要那一點在畫面外，raster position 就失效，整個字都不會畫出，而不是只裁掉超出的部分。
//
// 因此先把 raster position 設在一定有效的 viewport 中心，再用 glBitmap 位移到目標位置：
// glBitmap 的位移不經過裁切，移到畫面外仍然有效，畫出的像素再由 OpenGL 正常裁掉。
// glBitmap 以 window 像素位移，不套用座標轉換；字形 bitmap 本來就以像素繪製，不受縮放影響。
class RasterCursor {
   public:
    // 把 raster position 移到 origin（套用目前的座標轉換），之後的位置都相對於 origin。
    explicit RasterCursor(const Point& origin) {
        GLdouble modelview[16];
        GLdouble projection[16];
        GLint viewport[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        // 與 OpenGL 相同的投影計算：clip = projection × modelview × (x, y, 0, 1)，矩陣為 column-major。
        GLdouble eye[4];
        for (int i = 0; i < 4; i++) {
            eye[i] = modelview[i] * origin.x + modelview[4 + i] * origin.y + modelview[12 + i];
        }
        GLdouble clip[4];
        for (int i = 0; i < 4; i++) {
            clip[i] = projection[i] * eye[0] + projection[4 + i] * eye[1] + projection[8 + i] * eye[2] +
                      projection[12 + i] * eye[3];
        }

        // NDC 的 [-1, 1] 對應 viewport 的範圍。window 座標取整到整數像素：直接用 glRasterPos 時，
        // 投影的誤差可能讓 81 變成 80.9999924，GLUT 畫 bitmap 時取整就會偏左 1 px；取整後之後的整數位移也都是精確的。
        const double windowX = std::round(viewport[0] + (clip[0] / clip[3] + 1.0) * viewport[2] / 2.0);
        const double windowY = std::round(viewport[1] + (clip[1] / clip[3] + 1.0) * viewport[3] / 2.0);

        // 位移量相對於 viewport 中心（寬高為奇數時中心落在半像素上，位移量仍是精確的）。
        const auto dx = static_cast<GLfloat>(windowX - (viewport[0] + viewport[2] / 2.0));
        const auto dy = static_cast<GLfloat>(windowY - (viewport[1] + viewport[3] / 2.0));

        // 座標轉換全設為單位矩陣時，(0, 0) 就是 viewport 中心。
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glRasterPos2f(0.0f, 0.0f);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glPopAttrib();

        glBitmap(0, 0, 0.0f, 0.0f, dx, dy, nullptr);
        glGetFloatv(GL_CURRENT_RASTER_POSITION, _origin);
    }

    // 移到相對於 origin 的 (x, y)，y 向下為正。
    // 每次都從目前實際的位置計算位移，因此 glutBitmapCharacter 畫完自行推進 raster position 也不影響；
    // 位置一律由呼叫端依字寬決定，與 getFontWidth 的量測一致。
    void moveTo(float x, float y) {
        GLfloat current[4];
        glGetFloatv(GL_CURRENT_RASTER_POSITION, current);

        // window 座標 y 向上，因此 y 反向。
        glBitmap(0, 0, 0.0f, 0.0f, _origin[0] + x - current[0], _origin[1] - y - current[1], nullptr);
    }

   private:
    GLfloat _origin[4];  // origin 的 window 座標
};

#pragma endregion  // Raster position

#pragma region Renderers

// 每種字型一個 operator()，由 std::visit 分派；FontStyle 新增型別而沒有對應的多載時會編譯失敗。
class TextRenderer {
   public:
    TextRenderer(Canvas& canvas, const std::string& text, const Point& position, TextAnchor anchor)
        : _canvas(canvas), _text(text), _position(position), _anchor(anchor) {}

    void operator()(const GlyphFontStyle& style) const {
        // 排版一次，同時得到文字框大小與每個字形的位置。
        const TextLayout layout = fonts().get(style).layout(_text);
        if (layout.glyphs.empty()) {
            return;
        }

        const Point origin = alignedOrigin(_position, _anchor, layout.width, layout.height);

        // glDrawPixels 不套用 glColor。以 GL_ALPHA 格式送出時 RGB 視為 0，
        // 因此用 pixel transfer 的 bias 填入文字顏色、scale 乘上文字透明度，直接畫字形的 alpha bitmap。
        GLfloat color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);

        glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT);
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // bitmap 每列緊密排列，沒有對齊用的填充

        glPixelTransferf(GL_RED_BIAS, color[0]);
        glPixelTransferf(GL_GREEN_BIAS, color[1]);
        glPixelTransferf(GL_BLUE_BIAS, color[2]);
        glPixelTransferf(GL_ALPHA_SCALE, color[3]);

        RasterCursor cursor(origin);
        for (const PlacedGlyph& placed : layout.glyphs) {
            const Glyph& glyph = placed.glyph;

            // bitmap 的第一列是頂端；負的 Y zoom 讓 OpenGL 由上往下畫，像素字型放大時每個像素畫成 scale × scale。
            const auto scale = static_cast<float>(glyph.scale);
            glPixelZoom(scale, -scale);

            cursor.moveTo(placed.x, placed.y);
            glDrawPixels(glyph.bitmapWidth, glyph.bitmapHeight, GL_ALPHA, GL_UNSIGNED_BYTE, glyph.alpha);
        }

        glPopClientAttrib();
        glPopAttrib();
    }

    void operator()(const BitmapFontStyle& style) const {
        void* font = mapFont(style.font);

        const Point origin = alignedOrigin(_position, _anchor, getFontWidth(style, _text), getFontHeight(style, _text));

        const float lineHeight = getLineHeight(style.font);
        const float baseline = lineHeight - getOriginY(style.font);  // 第一行基線相對於文字框頂端

        RasterCursor cursor(origin);

        float x = 0.0f;
        float y = baseline;
        for (const char ch : _text) {
            const auto c = static_cast<unsigned char>(ch);

            if (c == '\n') {
                x = 0.0f;
                y += lineHeight;
                continue;
            }

            cursor.moveTo(x, y);
            glutBitmapCharacter(font, c);

            x += static_cast<float>(glutBitmapWidth(font, c));
        }
    }

    void operator()(const StrokeFontStyle& style) const {
        void* font = mapFont(style.font);

        const Point origin = alignedOrigin(_position, _anchor, getFontWidth(style, _text), getFontHeight(style, _text));

        const float scale = style.scale;
        const float lineHeight = getLineHeight(style.font) * scale;

        float x = origin.x;
        float y = origin.y + getAscent(style.font) * scale;

        glLineWidth(1.0f);

        for (const char ch : _text) {
            const auto c = static_cast<unsigned char>(ch);

            if (c == '\n') {
                x = origin.x;
                y += lineHeight;
                continue;
            }

            // Stroke 字型只有 ASCII，與 getFontWidth 相同，其餘位元組不畫也不佔寬度。
            if (c >= 128) {
                continue;
            }

            _canvas.pushTransform();
            _canvas.translate(x, y);

            // Stroke 字型以 y 向上的字型座標繪製，翻轉 Y 配合畫布 y 向下的座標系。
            glScalef(scale, -scale, 1.0f);

            // glutStrokeCharacter 畫完會把矩陣平移字寬，因此每個字都恢復矩陣，由 x 自行累加。
            glutStrokeCharacter(font, c);

            _canvas.popTransform();

            x += static_cast<float>(glutStrokeWidth(font, c)) * scale;
        }
    }

   private:
    Canvas& _canvas;
    const std::string& _text;
    Point _position;
    TextAnchor _anchor;
};

#pragma endregion  // Renderers

}  // namespace

void Canvas::drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color,
                      const Point& position, TextAnchor anchor) {
    if (text.empty()) {
        return;
    }

    // 避免影響其他元素的顏色、raster position 與線寬。
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glColor4f(color.r, color.g, color.b, color.a);

    std::visit(TextRenderer(*this, text, position, anchor), fontStyle);

    glPopAttrib();
}

}  // namespace robot::graphics
