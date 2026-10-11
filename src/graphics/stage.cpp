#include "graphics/stage.hpp"

#include "platform/glut.hpp"

namespace robot::graphics {

void begin3D(int width, int height, const Camera& camera, const OrthoProjection& projection) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 零尺寸視窗時 glOrtho 的範圍無效
    if (width > 0 && height > 0) {
        const OrthoProjection fitted = projection.fittedTo(static_cast<float>(width) / height);
        glOrtho(fitted.left, fitted.right, fitted.bottom, fitted.top, fitted.nearPlane, fitted.farPlane);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.focus.x, camera.focus.y, camera.focus.z, camera.up.x,
              camera.up.y, camera.up.z);

    glEnable(GL_DEPTH_TEST);
}

void begin2D(int width, int height) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 上下界對調：原點在左上，y 向下
    if (width > 0 && height > 0) {
        glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

}  // namespace robot::graphics
