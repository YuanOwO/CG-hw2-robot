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

}  // namespace

void SkeletonRenderer::render(const world::entity::LivingEntity& entity) const {
    using world::entity::anatomy::BoneIndex;
    using world::entity::anatomy::ROOT_BONE;

    const auto& skeleton = entity.rig().skeleton();
    std::vector<Mat4> transforms;
    entity.computeBoneTransforms(transforms);

    // 畫出實體的位置與朝向，方便看出骨架的相對位置
    glPushMatrix();
    glTranslatef(entity.position().x, entity.position().y, entity.position().z);
    glRotatef(entity.yaw(), 0.0f, 1.0f, 0.0f);
    graphics::drawAxes(0.5f);
    glPopMatrix();

    // 變換矩陣已經是世界座標，骨頭從關節畫到末端，關節畫成點
    std::vector<graphics::LineSegment> bones;
    std::vector<Vec3> joints;
    for (BoneIndex i = ROOT_BONE + 1; i < skeleton.boneCount(); ++i) {
        const Vec3 joint = transformPoint(transforms[i], Vec3());
        bones.push_back({joint, transformPoint(transforms[i], skeleton.bone(i).shape.tail)});
        joints.push_back(joint);
    }
    graphics::drawLines(bones, BONE_COLOR, BONE_WIDTH);
    graphics::drawPoints(joints, JOINT_COLOR, JOINT_SIZE);
}

}  // namespace robot::view::entity
