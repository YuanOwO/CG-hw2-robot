#include "render/renderer.hpp"

#include "platform/glut.hpp"

namespace robot::render {

void Renderer::beginFrame(int width, int height, const ColorRGBA& clearColor) {
    _width = width;
    _height = height;

    glDisable(GL_SCISSOR_TEST);  // 確保清除整個視窗
    glViewport(0, 0, width, height);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::begin2D() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, _width, _height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // glOrtho 的左右、上下界不能相等；零尺寸視窗沒有可繪製的區域，保留單位矩陣即可。
    if (_width > 0 && _height > 0) {
        glOrtho(0.0, static_cast<GLdouble>(_width), static_cast<GLdouble>(_height), 0.0, -1.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    _canvas.setSurfaceSize(_width, _height);
}

}  // namespace robot::render
