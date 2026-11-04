#include "graphics/2d/canvas.hpp"

#include "platform/glut.hpp"

namespace robot::graphics {

void Canvas::pushTransform() {
    glPushMatrix();
}

void Canvas::popTransform() {
    glPopMatrix();
}

void Canvas::translate(int x, int y) {
    glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f);
}

void Canvas::fillRect(int x, int y, int width, int height, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glRecti(x, y, x + width, y + height);
    glPopAttrib();
}

void Canvas::strokeRect(int x, int y, int width, int height, int thickness, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || thickness <= 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);

    // 上下邊框整列填滿
    glRecti(x, y, x + width, y + thickness);
    glRecti(x, y + height - thickness, x + width, y + height);

    // 左右邊框整列填滿
    glRecti(x, y + thickness, x + thickness, y + height - thickness);
    glRecti(x + width - thickness, y + thickness, x + width, y + height - thickness);

    glPopAttrib();
}

}  // namespace robot::graphics
