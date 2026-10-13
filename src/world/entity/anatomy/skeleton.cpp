#include "world/entity/anatomy/skeleton.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

#include "world/entity/anatomy/pose.hpp"

namespace robot::world::entity::anatomy {

Skeleton::Skeleton() {
    _bones.push_back({{"root", Vec3(), Vec3(), Vec3(), Vec3()}, NO_BONE, {}});  // 根骨頭
}

const Bone& Skeleton::bone(BoneIndex index) const {
    return _bones[index];
}

const Marker& Skeleton::marker(MarkerIndex index) const {
    return _markers[index];
}

BoneIndex Skeleton::findBone(const char* name) const {
    for (BoneIndex i = 0; i < boneCount(); ++i) {
        if (std::strcmp(_bones[i].shape.name, name) == 0) {
            return i;
        }
    }
    throw std::invalid_argument("No bone named " + std::string(name));
}

const Dof& Skeleton::dof(BoneIndex boneIndex, int dofIndex) const {
    return _bones[boneIndex].dofs[dofIndex];
}

BoneIndex Skeleton::addBone(const Bone& bone) {
    if (bone.parent < 0 || bone.parent >= boneCount()) {
        throw std::invalid_argument("Parent must be a valid bone index.");
    }

    if (bone.dofs.size() > MAX_DOFS) {
        throw std::invalid_argument("Too many degrees of freedom.");
    }

    for (const Dof& dof : bone.dofs) {
        if (dof.min > dof.max) {
            throw std::invalid_argument("DOF range is inverted.");
        }
        if (lengthSquared(dof.axis) < EPSILON) {
            throw std::invalid_argument("DOF axis is zero.");
        }
    }

    _bones.push_back(bone);
    return static_cast<BoneIndex>(_bones.size() - 1);
}

MarkerIndex Skeleton::addMarker(const Marker& marker) {
    if (marker.bone < 0 || marker.bone >= boneCount()) {
        throw std::invalid_argument("Marker refers to a missing bone.");
    }

    _markers.push_back(marker);
    return static_cast<MarkerIndex>(_markers.size() - 1);
}

void Skeleton::computeTransforms(const Pose& pose, const Mat4& root, std::vector<Mat4>& out) const {
    out.resize(_bones.size());

    out[0] = root;

    for (auto i = 1; i < boneCount(); i++) {
        const Bone& bone = _bones[i];

        out[i] = out[bone.parent];          // 繼承父骨頭的變換矩陣
        out[i] *= translation(bone.shape.pivot);  // 移動到旋轉中心
        for (int j = 0; j < bone.dofs.size(); ++j) {
            const Dof& dof = bone.dofs[j];
            out[i] *= rotation(pose.angle(i, j), dof.axis);  // 旋轉
        }
    }
}

Vec3 Skeleton::markerPosition(const std::vector<Mat4>& transforms, MarkerIndex index) const {
    const Marker& marker = _markers[index];
    const Mat4& boneTransform = transforms[marker.bone];
    return transformPoint(boneTransform, marker.offset);
}

}  // namespace robot::world::entity::anatomy
