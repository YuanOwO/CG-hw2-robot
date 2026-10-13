#include "world/entity/anatomy/upright_quadruped_rig.hpp"

#include <cmath>

namespace robot::world::entity::anatomy {

namespace {

const float EYE_HEIGHT = 0.75f;  // 頭高度的比例

struct LegLayout {
    const char* upper;
    const char* lower;
    float side;   // +1 為左（+x）
    float front;  // +1 為前（+z）
    float phase;  // 圈，對角的兩條腿同步
};
const LegLayout LEGS[] = {
    {"leg_front_left_upper",  "leg_front_left_lower",  1.0f,  1.0f,  0.5f},
    {"leg_front_right_upper", "leg_front_right_lower", -1.0f, 1.0f,  0.0f},
    {"leg_back_left_upper",   "leg_back_left_lower",   1.0f,  -1.0f, 0.0f},
    {"leg_back_right_upper",  "leg_back_right_lower",  -1.0f, -1.0f, 0.5f},
};

}  // namespace

UprightQuadrupedRig::UprightQuadrupedRig(const UprightQuadrupedSize& size, const QuadrupedGait& gait) {
    const float hipY = size.upperLeg.y + size.lowerLeg.y;  // 腿站在地上，髖在腿頂
    const Vec3 halfBody = size.body / 2.0f;
    const Vec3 halfHead = size.head / 2.0f;
    const Vec3 halfLower = size.lowerLeg / 2.0f;

    const BoneIndex torso =
        add<Torso>(ROOT_BONE, BoneShape{"torso", Vec3(0.0f, hipY, 0.0f), Vec3(0.0f, size.body.y, 0.0f),
                                        Vec3(-halfBody.x, 0.0f, -halfBody.z), Vec3(halfBody.x, size.body.y, halfBody.z)})
            .bone();

    addMainHead(torso, {"head", Vec3(0.0f, size.body.y, 0.0f), Vec3(0.0f, size.head.y, 0.0f),
                        Vec3(-halfHead.x, 0.0f, -halfHead.z), Vec3(halfHead.x, size.head.y, halfHead.z)});
    addEye(Vec3(0.0f, size.head.y * EYE_HEIGHT, halfHead.z));

    // 腿貼齊軀幹左右兩側。轉軸在軀幹底面的前緣或後緣，腿整個凸出在軀幹外，
    // 只以這條邊和軀幹相接，擺動時才不會離開身體
    const Vec3 halfLeg = size.upperLeg / 2.0f;
    for (const LegLayout& layout : LEGS) {
        const LimbMotion motion = {layout.phase, gait.swing};
        const float farZ = layout.front * size.upperLeg.z;
        const BoneShape upper = {
            layout.upper,
            Vec3(layout.side * (halfBody.x - halfLeg.x), 0.0f, layout.front * halfBody.z),
            Vec3(0.0f, -size.upperLeg.y, farZ / 2.0f),
            Vec3(-halfLeg.x, -size.upperLeg.y, std::fmin(0.0f, farZ)),
            Vec3(halfLeg.x, 0.0f, std::fmax(0.0f, farZ)),
        };
        if (size.lowerLeg != Vec3()) {  // 零向量表示只有一節
            const BoneShape lower = {layout.lower, upper.tail, Vec3(0.0f, -size.lowerLeg.y, 0.0f),
                                     Vec3(-halfLower.x, -size.lowerLeg.y, -halfLower.z),
                                     Vec3(halfLower.x, 0.0f, halfLower.z)};
            add<Limb>(torso, upper, lower, motion, gait.strideLength);
        } else {
            add<Limb>(torso, upper, motion, gait.strideLength);
        }
    }
}

}  // namespace robot::world::entity::anatomy
