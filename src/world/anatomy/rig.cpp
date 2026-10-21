#include "world/anatomy/rig.hpp"

#include <stdexcept>

#include "math/vec3.hpp"

namespace robot::world::anatomy {

BoneIndex Rig::findBone(std::string_view name) const {
    for (BoneIndex i = 0; i < boneCount(); i++) {
        if (_bones[i].name() == name) {
            return i;
        }
    }
    return NO_BONE;
}

// const Pose& Rig::stancePose(Stance stance) const {
//     return _stanceAngles.at(static_cast<std::size_t>(stance));
// }

// const collision::UprightCylinder& Rig::stanceCollider(Stance stance) const {
//     return _stanceColliders.at(static_cast<std::size_t>(stance));
// }

void Rig::clampPose(Pose& pose) const {
    if (pose.boneCount() != boneCount()) {
        throw std::invalid_argument("Pose bone count does not match rig bone count");
    }

    for (BoneIndex i = 0; i < boneCount(); i++) {
        const Joint& joint = _bones[i].joint();
        JointAngles& angles = pose[i];

        angles.yaw = joint.yaw().clamp(angles.yaw);
        angles.pitch = joint.pitch().clamp(angles.pitch);
        angles.roll = joint.roll().clamp(angles.roll);
    }
}

std::vector<Mat4> Rig::transforms(Pose pose) const {
    if (pose.boneCount() != boneCount()) {
        throw std::invalid_argument("Pose bone count does not match rig bone count");
    }

    clampPose(pose);

    std::vector<Mat4> ret(boneCount());

    for (BoneIndex i = 1; i < boneCount(); i++) {
        const Bone& bone = _bones[i];
        const JointAngles& angles = pose[i];
        const Mat4& parent = ret[bone.parent()];

        ret[i] = parent * translation(bone.pivot()) * rotation(angles.yaw, Vec3(0.0f, 1.0f, 0.0f)) *
                 rotation(angles.pitch, Vec3(-1.0f, 0.0f, 0.0f)) * rotation(angles.roll, Vec3(0.0f, 0.0f, 1.0f));
    }

    return ret;
}

}  // namespace robot::world::anatomy
