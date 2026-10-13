#include "view/entity/skeleton_renderer.hpp"

#include <vector>

#include "common/color.hpp"
#include "graphics/primitives.hpp"
#include "platform/glut.hpp"

namespace robot::view::entity {

namespace {

const ColorRGBA BONE_COLOR(1.0f, 0.85f, 0.2f);
const ColorRGBA JOINT_COLOR(0.9f, 0.2f, 0.2f);
const float BONE_WIDTH = 3.0f;  // 像素
const float JOINT_SIZE = 8.0f;  // 像素

void vertex(const Vec3& point) {
    glVertex3f(point.x, point.y, point.z);
}

}  // namespace

void SkeletonRenderer::render(const world::entity::LivingEntity& entity) const {
    using world::entity::anatomy::BoneIndex;
    using world::entity::anatomy::ROOT_BONE;

    const auto& skeleton = entity.rig().skeleton();
    std::vector<Mat4> transforms;
    entity.computeBoneTransforms(transforms);

    // 關掉深度測試，骨架才不會被地面或其他東西擋住
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT);
    // glDisable(GL_DEPTH_TEST);

    // 畫出實體的位置與朝向，方便看出骨架的相對位置
    glPushMatrix();
    glTranslatef(entity.position().x, entity.position().y, entity.position().z);
    glRotatef(entity.yaw(), 0.0f, 1.0f, 0.0f);
    graphics::drawAxes(0.5f);
    glPopMatrix();

    // 畫出骨架
    glLineWidth(BONE_WIDTH);
    glColor4f(BONE_COLOR.r, BONE_COLOR.g, BONE_COLOR.b, BONE_COLOR.a);
    glBegin(GL_LINES);
    for (BoneIndex i = ROOT_BONE + 1; i < skeleton.boneCount(); ++i) {
        vertex(transformPoint(transforms[i], Vec3()));
        vertex(transformPoint(transforms[i], skeleton.bone(i).shape.tail));
    }
    glEnd();

    // 畫出關節
    glPointSize(JOINT_SIZE);
    glColor4f(JOINT_COLOR.r, JOINT_COLOR.g, JOINT_COLOR.b, JOINT_COLOR.a);
    glBegin(GL_POINTS);
    for (BoneIndex i = ROOT_BONE + 1; i < skeleton.boneCount(); ++i) {
        vertex(transformPoint(transforms[i], Vec3()));
    }
    glEnd();

    glPopAttrib();
}

}  // namespace robot::view::entity
