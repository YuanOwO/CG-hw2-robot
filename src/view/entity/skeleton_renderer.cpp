#include "view/entity/skeleton_renderer.hpp"

#include <cmath>
#include <vector>

#include "common/color.hpp"
#include "graphics/primitives.hpp"
#include "math/utils.hpp"
#include "platform/glut.hpp"

namespace robot::view::entity {

namespace {

const ColorRGBA BONE_COLOR(1.0f, 0.85f, 0.2f);
const ColorRGBA JOINT_COLOR(0.9f, 0.2f, 0.2f);
const ColorRGBA BOUNDS_COLOR(1.0f, 1.0f, 1.0f);
const ColorRGBA MARKER_COLOR(0.2f, 0.4f, 1.0f);
const ColorRGBA GAZE_COLOR(0.2f, 0.9f, 1.0f);
const float BONE_WIDTH = 3.0f;    // 像素
const float JOINT_SIZE = 8.0f;    // 像素
const float BOUNDS_WIDTH = 1.0f;  // 像素
const float MARKER_SIZE = 6.0f;   // 像素
const float GAZE_WIDTH = 2.0f;    // 像素
const float GAZE_LENGTH = 2.0f;   // 公尺

// 實體的視線方向：yaw 為 0 時面向 +z、正值逆時針，pitch 正值往上
Vec3 gazeDirection(float yaw, float pitch) {
    const float radiansYaw = radians(yaw);
    const float radiansPitch = radians(pitch);
    return Vec3(std::sin(radiansYaw) * std::cos(radiansPitch), std::sin(radiansPitch),
                std::cos(radiansYaw) * std::cos(radiansPitch));
}

// 把骨頭座標系裡的方塊換到世界座標，加入它的 12 條邊。骨頭會轉，所以方塊不一定和世界的軸對齊
void addBoxEdges(std::vector<graphics::LineSegment>& edges, const Mat4& transform, const Vec3& min,
                 const Vec3& max) {
    // 第 i 個角的 x、y、z 依 i 的第 0、1、2 個位元取 min 或 max
    Vec3 corners[8];
    for (int i = 0; i < 8; ++i) {
        const Vec3 corner((i & 1) ? max.x : min.x, (i & 2) ? max.y : min.y, (i & 4) ? max.z : min.z);
        corners[i] = transformPoint(transform, corner);
    }
    // 只差一個位元的兩個角相連
    for (int i = 0; i < 8; ++i) {
        for (int bit : {1, 2, 4}) {
            if (!(i & bit)) {
                edges.push_back({corners[i], corners[i | bit]});
            }
        }
    }
}

}  // namespace

void SkeletonRenderer::render(const world::entity::LivingEntity& entity) const {
    using world::entity::anatomy::BoneIndex;
    using world::entity::anatomy::MarkerIndex;
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
    std::vector<graphics::LineSegment> bounds;
    std::vector<Vec3> joints;
    for (BoneIndex i = ROOT_BONE + 1; i < skeleton.boneCount(); ++i) {
        const auto& shape = skeleton.bone(i).shape;
        const Vec3 joint = transformPoint(transforms[i], Vec3());
        bones.push_back({joint, transformPoint(transforms[i], shape.tail)});
        joints.push_back(joint);
        addBoxEdges(bounds, transforms[i], shape.boundsMin, shape.boundsMax);
    }
    graphics::drawLines(bounds, BOUNDS_COLOR, BOUNDS_WIDTH);
    graphics::drawLines(bones, BONE_COLOR, BONE_WIDTH);
    graphics::drawPoints(joints, JOINT_COLOR, JOINT_SIZE);

    std::vector<Vec3> markers;
    for (MarkerIndex i = 0; i < skeleton.markerCount(); ++i) {
        markers.push_back(skeleton.markerPosition(transforms, i));
    }
    graphics::drawPoints(markers, MARKER_COLOR, MARKER_SIZE);

    // 視線是實體的 yaw 與 pitch，不是頭實際的朝向：頭轉不到的角度（例如沒有脖子的生物）也看得出來
    if (entity.rig().eye() != world::entity::anatomy::NO_MARKER) {
        const Vec3 eye = skeleton.markerPosition(transforms, entity.rig().eye());
        const Vec3 gaze = eye + gazeDirection(entity.yaw(), entity.pitch()) * GAZE_LENGTH;
        graphics::drawLines({{eye, gaze}}, GAZE_COLOR, GAZE_WIDTH);
    }
}

}  // namespace robot::view::entity
