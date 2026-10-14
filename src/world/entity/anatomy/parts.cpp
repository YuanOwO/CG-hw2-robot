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
const JointLimits WING_LIMITS = {0.0f, 0.0f, 90.0f};

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

// 下節繞著上節底面的中央彎曲，頂面的角會離開上節，在彎曲的外側裂開一道縫。
// 把下節往上延伸厚度的一半蓋住關節，彎到 90 度左右都還接得上
Limb::Limb(Skeleton& skeleton, BoneIndex parent, const BoneShape& upper, BoneShape lower, const LimbMotion& motion,
           float strideLength)
    : Limb(skeleton, parent, upper, motion, strideLength) {
    lower.boundsMax.y += (lower.boundsMax.z - lower.boundsMin.z) / 2.0f;
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

#pragma region Wing

Wing::Wing(Skeleton& skeleton, BoneIndex parent, const BoneShape& shape, const WingMotion& motion, float strideLength)
    : _bone(skeleton.addBone({shape, parent, jointDofs(WING_LIMITS)})), _motion(motion), _strideLength(strideLength) {
    if (strideLength <= 0.0f) {
        throw std::invalid_argument("Stride length must be positive.");
    }
}

// 往下垂的翅膀繞 +z 正轉時，下緣往 +x 甩出去，所以左翅張開是正的 roll、右翅是負的。
// 張開的程度在 0 到 1 之間平滑來回，不會往內穿進身體
void Wing::animate(const AnimationInput& input, Pose& pose) const {
    const float cycle = input.walkDistance / _strideLength * _motion.beatsPerStride;
    const float open = (1.0f - std::cos(2.0f * PI * cycle)) / 2.0f * input.walkAmount;
    pose.setAngle(_bone, ROLL, _motion.side * _motion.flap * open);
}

#pragma endregion  // Wing

}  // namespace robot::world::entity::anatomy
