#include "world/entity/anatomy/humanoid_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

struct ArmLayout {
    const char* upper;
    const char* lower;
    float side;   // +1 為左（+x）
    float phase;  // 圈，與同側的腿相反
};
const ArmLayout ARMS[] = {
    {"arm_left_upper",  "arm_left_lower",  1.0f,  0.5f},
    {"arm_right_upper", "arm_right_lower", -1.0f, 0.0f},
};

}  // namespace

HumanoidRig::HumanoidRig(const HumanoidSize& size, const HumanoidGait& gait) : BipedRig(size, gait) {
    const Vec3 halfHead = size.head / 2.0f;
    addMainHead(torso(), {"head", Vec3(0.0f, size.body.y, 0.0f), Vec3(0.0f, size.head.y, 0.0f),
                          Vec3(-halfHead.x, 0.0f, -halfHead.z), Vec3(halfHead.x, size.head.y, halfHead.z)});
    addEye(Vec3(0.0f, halfHead.y, halfHead.z));  // 臉的正面，頭高度的中間

    // 手臂貼在軀幹兩側，頂端與軀幹頂端齊平；手往前擺時手肘多彎，也就是正的 pitch
    const float shoulderX = (size.body.x + size.upperArm.x) / 2.0f;
    const Vec3 halfUpper = size.upperArm / 2.0f;
    const Vec3 halfLower = size.lowerArm / 2.0f;
    for (const ArmLayout& layout : ARMS) {
        const LimbMotion motion = {layout.phase, gait.armSwing, gait.elbowRest, gait.elbowBend, 0.0f};
        const BoneShape upper = {layout.upper, Vec3(layout.side * shoulderX, size.body.y, 0.0f),
                                 Vec3(0.0f, -size.upperArm.y, 0.0f), Vec3(-halfUpper.x, -size.upperArm.y, -halfUpper.z),
                                 Vec3(halfUpper.x, 0.0f, halfUpper.z)};
        if (size.lowerArm != Vec3()) {  // 零向量表示只有一節
            const BoneShape lower = {layout.lower, upper.tail, Vec3(0.0f, -size.lowerArm.y, 0.0f),
                                     Vec3(-halfLower.x, -size.lowerArm.y, -halfLower.z),
                                     Vec3(halfLower.x, 0.0f, halfLower.z)};
            add<Limb>(torso(), upper, lower, motion, gait.strideLength);
        } else {
            add<Limb>(torso(), upper, motion, gait.strideLength);
        }
    }
}

}  // namespace robot::world::entity::anatomy
