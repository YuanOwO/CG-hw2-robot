#include "world/entity/anatomy/pose.hpp"

#include <algorithm>

namespace robot::world::entity::anatomy {

Pose::Pose(const Skeleton& skeleton) : _skeleton(skeleton) {
    _angles.resize(skeleton.boneCount());
    for (BoneIndex i = 0; i < skeleton.boneCount(); ++i) {
        const Bone& bone = skeleton.bone(i);
        for (int j = 0; j < bone.dofs.size(); ++j) {
            const Dof& dof = bone.dofs[j];
            // 從最接近 0 的合法值開始
            if (dof.min > 0.0f) {
                _angles[i][j] = dof.min;
            } else if (dof.max < 0.0f) {
                _angles[i][j] = dof.max;
            } else {
                _angles[i][j] = 0.0f;
            }
        }
    }
}

float Pose::angle(BoneIndex boneIndex, int dofIndex) const {
    return _angles[boneIndex][dofIndex];
}

void Pose::setAngle(BoneIndex boneIndex, int dofIndex, float degrees) {
    const Dof& dof = _skeleton.dof(boneIndex, dofIndex);
    _angles[boneIndex][dofIndex] = std::clamp(degrees, dof.min, dof.max);
}

}  // namespace robot::world::entity::anatomy
