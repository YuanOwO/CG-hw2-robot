#include "world/entity/anatomy/parts.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "math/constants.hpp"
#include "world/entity/anatomy/pose.hpp"

namespace robot::world::entity::anatomy {

namespace {

const Vec3 X(1.0f, 0.0f, 0.0f);
const Vec3 Y(0.0f, 1.0f, 0.0f);
const Vec3 Z(0.0f, 0.0f, 1.0f);

// 肢體關節本身能轉的範圍，比走路的擺幅寬，之後其他動作才不會被夾住
const JointLimits SWING_LIMITS = {0.0f, 90.0f, 0.0f};
const JointLimits BEND_LIMITS = {0.0f, 150.0f, 0.0f};

// 依 JointAxis 的順序：先轉 yaw 再轉 pitch，低頭時才不會連帶歪向一邊。
// 範圍為 0 的軸仍保留 DOF，角度會被夾在 0，每根骨頭的索引因此都一樣
std::vector<Dof> jointDofs(const JointLimits& limits) {
    return {
        {"yaw",   Y,  -limits.yaw,   limits.yaw  },
        {"pitch", -X, -limits.pitch, limits.pitch},
        {"roll",  Z,  -limits.roll,  limits.roll },
    };
}

}  // namespace

#pragma region Head

Head::Head(Skeleton& skeleton, BoneIndex parent, const BoneShape& shape, const JointLimits& limits)
    : _bone(skeleton.addBone({shape, parent, jointDofs(limits)})), _limits(limits) {}

void Head::animate(const AnimationInput& input, Pose& pose) const {
    pose.setAngle(_bone, YAW, input.lookYaw);
    pose.setAngle(_bone, PITCH, input.lookPitch);
}

#pragma endregion  // Head

#pragma region Torso

Torso::Torso(Skeleton& skeleton, BoneIndex parent, const BoneShape& shape, const JointLimits& limits)
    : _bone(skeleton.addBone({shape, parent, jointDofs(limits)})) {}

#pragma endregion  // Torso

#pragma region Limb

Limb::Limb(Skeleton& skeleton, BoneIndex parent, const BoneShape& upper, const LimbMotion& motion, float strideLength)
    : _motion(motion), _strideLength(strideLength) {
    if (strideLength <= 0.0f) {
        throw std::invalid_argument("Stride length must be positive.");
    }
    _upper = skeleton.addBone({upper, parent, jointDofs(SWING_LIMITS)});
}

Limb::Limb(Skeleton& skeleton, BoneIndex parent, const BoneShape& upper, const BoneShape& lower,
           const LimbMotion& motion, float strideLength)
    : Limb(skeleton, parent, upper, motion, strideLength) {
    _lower = skeleton.addBone({lower, _upper, jointDofs(BEND_LIMITS)});
}

// 相位跟著走過的距離，停下時不再前進；擺幅跟著 walkAmount 慢慢收回，肢體才不會瞬間歸位。
void Limb::animate(const AnimationInput& input, Pose& pose) const {
    const float cycle = input.walkDistance / _strideLength + _motion.phase;
    const float s = std::sin(2.0f * PI * cycle) * input.walkAmount;

    pose.setAngle(_upper, PITCH, _motion.swing * s);
    if (_lower != NO_BONE) {
        const float bend = _motion.bendFront * std::max(s, 0.0f) + _motion.bendBack * std::max(-s, 0.0f);
        pose.setAngle(_lower, PITCH, _motion.bendRest + bend);
    }
}

#pragma endregion  // Limb

}  // namespace robot::world::entity::anatomy
