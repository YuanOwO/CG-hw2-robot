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

    auto transforms = entity.skeleton().transforms(entity.pose());

    for (size_t i = 1; i < entity.skeleton().boneCount(); i++) {
        const auto& bone = entity.skeleton().bone(i);

        glPushMatrix();
        glMultMatrixf(transforms[i].data());

        if (bone.mesh().has_value()) {
            graphics::drawMesh(bone.mesh().value());
        } else if (bone.collider().has_value()) {
            drawCollider(bone.collider().value());
        }

        glPopMatrix();
    }

    glPopMatrix();
}

}  // namespace robot::view
