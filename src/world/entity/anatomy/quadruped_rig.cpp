#include "world/entity/anatomy/quadruped_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

struct LegLayout {
    const char* upper;
    const char* lower;
    float side;   // +1 為左（+x）
    float front;  // +1 為前（+z）
    float phase;  // 圈，對角的兩條腿同步
};
// 依 LegSlot 的順序
const LegLayout LEGS[] = {
    {"leg_front_left_upper",  "leg_front_left_lower",  1.0f,  1.0f,  0.5f},
    {"leg_front_right_upper", "leg_front_right_lower", -1.0f, 1.0f,  0.0f},
    {"leg_back_left_upper",   "leg_back_left_lower",   1.0f,  -1.0f, 0.0f},
    {"leg_back_right_upper",  "leg_back_right_lower",  -1.0f, -1.0f, 0.5f},
};

const LegLayout& layoutOf(LegSlot slot) {
    return LEGS[static_cast<int>(slot)];
}

}  // namespace

QuadrupedRig::QuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait) : _size(size), _gait(gait) {
    const float hipY = size.upperLeg.y + size.lowerLeg.y;  // 腿站在地上，髖在腿頂
    const Vec3 halfBody = size.body / 2.0f;

    _torso = add<Torso>(ROOT_BONE,
                        BoneShape{"torso", Vec3(0.0f, hipY, 0.0f), Vec3(0.0f, size.body.y, 0.0f),
                                  Vec3(-halfBody.x, 0.0f, -halfBody.z), Vec3(halfBody.x, size.body.y, halfBody.z)})
                 .bone();
}

float QuadrupedRig::sideOf(LegSlot slot) {
    return layoutOf(slot).side;
}

float QuadrupedRig::frontOf(LegSlot slot) {
    return layoutOf(slot).front;
}

// 前腳的膝蓋往後彎，後腳的往前彎；和人一樣只在腿擺到後方時彎
void QuadrupedRig::addLeg(LegSlot slot, BoneShape upper) {
    const LegLayout& layout = layoutOf(slot);
    const float kneeBend = layout.front > 0.0f ? -_gait.frontKneeBend : _gait.backKneeBend;
    const LimbMotion motion = {layout.phase, _gait.swing, 0.0f, 0.0f, kneeBend};

    upper.name = layout.upper;
    if (_size.lowerLeg != Vec3()) {
        const Vec3 halfLower = _size.lowerLeg / 2.0f;
        const BoneShape lower = {layout.lower, upper.tail, Vec3(0.0f, -_size.lowerLeg.y, 0.0f),
                                 Vec3(-halfLower.x, -_size.lowerLeg.y, -halfLower.z),
                                 Vec3(halfLower.x, 0.0f, halfLower.z)};
        add<Limb>(_torso, upper, lower, motion, _gait.strideLength);
    } else {
        add<Limb>(_torso, upper, motion, _gait.strideLength);
    }
}

}  // namespace robot::world::entity::anatomy
