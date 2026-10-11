#include "view/world_renderer.hpp"

#include "platform/glut.hpp"
#include "graphics/stage.hpp"

namespace robot::view {

void WorldRenderer::render(const world::World& world, const graphics::Camera& camera,
                           const graphics::OrthoProjection& projection, int width, int height) const {
    graphics::begin3D(width, height, camera, projection);

    // 繞地板中心旋轉
    glTranslatef(25.0f, 0.0f, 25.0f);
    glRotatef(world.floorAngle(), 0.0f, 1.0f, 0.0f);
    glTranslatef(-25.0f, 0.0f, -25.0f);

    // 逆時針，正面朝上
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 50.0f);
    glVertex3f(50.0f, 0.0f, 50.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);
    glEnd();
}

}  // namespace robot::view
