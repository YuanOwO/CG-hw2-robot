#include "world/entity/anatomy/biped_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

struct LegLayout {
    const char* upper;
    const char* lower;
    float side;   // +1 為左（+x）
    float phase;  // 圈，左右腳交替
};
const LegLayout LEGS[] = {
    {"leg_left_upper",  "leg_left_lower",  1.0f,  0.0f},
    {"leg_right_upper", "leg_right_lower", -1.0f, 0.5f},
};

}  // namespace

BipedRig::BipedRig(const BipedSize& size, const BipedGait& gait) {
    const float hipY = size.upperLeg.y + size.lowerLeg.y;  // 腿站在地上，髖在腿頂
    const Vec3 halfBody = size.body / 2.0f;
    const Vec3 halfUpper = size.upperLeg / 2.0f;
    const float hipX = size.hipWidth / 2.0f;
    const Vec3 halfLower = size.lowerLeg / 2.0f;

    _torso = add<Torso>(ROOT_BONE,
                        BoneShape{"torso", Vec3(0.0f, hipY, 0.0f), Vec3(0.0f, size.body.y, 0.0f),
                                  Vec3(-halfBody.x, 0.0f, -halfBody.z), Vec3(halfBody.x, size.body.y, halfBody.z)})
                 .bone();

    // 腿往後擺時膝蓋才彎；往後彎是負的 pitch，往前彎是正的
    const float kneeBend = gait.knee == KneeDirection::Backward ? -gait.kneeBend : gait.kneeBend;
    for (const LegLayout& layout : LEGS) {
        const LimbMotion motion = {layout.phase, gait.swing, 0.0f, 0.0f, kneeBend};
        const BoneShape upper = {layout.upper, Vec3(layout.side * hipX, 0.0f, 0.0f),
                                 Vec3(0.0f, -size.upperLeg.y, 0.0f), Vec3(-halfUpper.x, -size.upperLeg.y, -halfUpper.z),
                                 Vec3(halfUpper.x, 0.0f, halfUpper.z)};
        if (size.lowerLeg != Vec3()) {  // 零向量表示只有一節
            const BoneShape lower = {layout.lower, upper.tail, Vec3(0.0f, -size.lowerLeg.y, 0.0f),
                                     Vec3(-halfLower.x, -size.lowerLeg.y, -halfLower.z),
                                     Vec3(halfLower.x, 0.0f, halfLower.z)};
            add<Limb>(_torso, upper, lower, motion, gait.strideLength);
        } else {
            add<Limb>(_torso, upper, motion, gait.strideLength);
        }
    }
}

}  // namespace robot::world::entity::anatomy
