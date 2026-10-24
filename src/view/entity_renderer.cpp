#include "view/entity_renderer.hpp"

#include "graphics/3d/primitives.hpp"
#include "platform/glut.hpp"
#include "view/collider_renderer.hpp"

namespace robot::view {

void EntityRenderer::render(const world::entity::Entity& entity) const {
    const auto& collider = entity.collider();
    drawCollider(collider);

    glPushMatrix();
    glTranslatef(entity.position().x, entity.position().y, entity.position().z);

    glRotatef(entity.yaw(), 0.0f, 1.0f, 0.0f);

    graphics::drawAxes(0.5f);

    glPopMatrix();
}

}  // namespace robot::view
