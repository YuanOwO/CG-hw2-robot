#include "view/obstacle_renderer.hpp"

#include "graphics/3d/primitives.hpp"
#include "platform/glut.hpp"
#include "view/collider_renderer.hpp"

namespace robot::view {

void ObstacleRenderer::render(const world::obstacle::Obstacle& obstacle) const {
    drawCollider(obstacle.collider());

    glPushMatrix();
    glTranslatef(obstacle.position().x, obstacle.position().y, obstacle.position().z);

    graphics::drawMesh(obstacle.mesh());

    glPopMatrix();
}

}  // namespace robot::view
