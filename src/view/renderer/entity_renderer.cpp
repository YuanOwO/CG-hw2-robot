#include "view/renderer/entity_renderer.hpp"

#include <cstddef>

#include "graphics/3d/primitives.hpp"
#include "platform/glut.hpp"
#include "view/renderer/collider_renderer.hpp"
#include "world/entity/robot.hpp"

namespace robot::view {

const animate::Animator* EntityRenderer::animatorFor(const world::entity::Entity& entity) const {
    if (dynamic_cast<const world::entity::Robot*>(&entity) != nullptr) {
        return &_robotAnimator;
    }
    return nullptr;
}

void EntityRenderer::render(const world::entity::Entity& entity) const {
    const auto& collider = entity.collider();
    drawCollider(collider);

    glPushMatrix();
    glTranslatef(entity.position().x, entity.position().y, entity.position().z);
    glRotatef(entity.yaw(), 0.0f, 1.0f, 0.0f);

    graphics::drawAxes(0.5f);

    const animate::Animator* animator = animatorFor(entity);
    const auto pose = animator != nullptr ? animator->animate(entity) : entity.pose();
    auto transforms = entity.skeleton().transforms(pose);

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
