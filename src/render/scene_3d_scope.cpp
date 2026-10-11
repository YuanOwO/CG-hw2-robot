#include "render/scene_3d_scope.hpp"

#include "platform/glut.hpp"

namespace robot::render {

Scene3DScope::Scene3DScope(RenderContext& context, const BoundingBox& area, const Camera& camera,
                           const OrthoProjection& projection) {
    // glOrtho 的左右、上下界不能相等，寬高比也需要非零的高度。
    if (area.width <= 0 || area.height <= 0) {
        return;
    }
    _active = true;

    // 場景繪製可能改變任何狀態（顏色、線寬、光照等），全部保存，解構時一次恢復。
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // 視窗座標以左上為原點、y 向下；OpenGL 的 viewport 以左下為原點、y 向上。
    const int glY = context.surfaceHeight() - area.y - area.height;
    glViewport(area.x, glY, area.width, area.height);

    // 之後的清除與繪製都限制在 area 內，不影響其他區域。
    glEnable(GL_SCISSOR_TEST);
    glScissor(area.x, glY, area.width, area.height);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    const OrthoProjection fitted = projection.fittedTo(static_cast<float>(area.width) / area.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(fitted.left, fitted.right, fitted.bottom, fitted.top, fitted.nearPlane, fitted.farPlane);

    // 視角轉換須緊接在 glLoadIdentity 之後、所有幾何轉換之前。
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.focus.x, camera.focus.y, camera.focus.z, camera.up.x,
              camera.up.y, camera.up.z);

    glEnable(GL_DEPTH_TEST);
}

Scene3DScope::~Scene3DScope() {
    if (!_active) {
        return;
    }

    // 先恢復兩個矩陣堆疊，再由 glPopAttrib 恢復矩陣模式與其他狀態。
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

}  // namespace robot::render
