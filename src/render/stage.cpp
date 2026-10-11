#include "render/stage.hpp"

#include "platform/glut.hpp"

namespace robot::render {

void begin3D(int width, int height, const Camera& camera, const OrthoProjection& projection) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // glOrtho 的左右、上下界不能相等，零尺寸視窗保留單位矩陣即可。
    if (width > 0 && height > 0) {
        const OrthoProjection fitted = projection.fittedTo(static_cast<float>(width) / height);
        glOrtho(fitted.left, fitted.right, fitted.bottom, fitted.top, fitted.nearPlane, fitted.farPlane);
    }

    // 視角轉換須緊接在 glLoadIdentity 之後、所有幾何轉換之前（講義 02-2）。
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.focus.x, camera.focus.y, camera.focus.z, camera.up.x,
              camera.up.y, camera.up.z);

    // 3D 物體依距離決定前後遮擋
    glEnable(GL_DEPTH_TEST);
}

void begin2D(int width, int height) {
    // 介面一律蓋在場景上，不和 3D 物體比較深度
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 上下界對調：y 向下為正，原點在左上，和視窗、滑鼠座標一致。
    // glOrtho 的左右、上下界不能相等，零尺寸視窗保留單位矩陣即可。
    if (width > 0 && height > 0) {
        glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

}  // namespace robot::render
