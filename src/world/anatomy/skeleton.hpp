#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "math/mat4.hpp"
#include "world/anatomy/bone.hpp"
#include "world/anatomy/pose.hpp"

namespace robot::world::anatomy {

class Skeleton {
    friend class SkeletonBuilder;

   public:
    Skeleton(const Skeleton&) = delete;
    Skeleton& operator=(const Skeleton&) = delete;
    Skeleton(Skeleton&&) = default;
    Skeleton& operator=(Skeleton&&) = default;

    std::size_t boneCount() const { return _bones.size(); }
    const Bone& bone(BoneIndex index) const { return _bones.at(index); }
    const std::vector<BoneIndex>& children(BoneIndex index) const { return _children.at(index); }
    BoneIndex findBone(std::string_view name) const;

    std::size_t markerCount() const { return _markers.size(); }
    const Marker& marker(std::size_t index) const { return _markers.at(index); }

    void clampPose(Pose& pose) const;

    std::vector<Mat4> transforms(Pose pose) const;

   private:
    Skeleton() = default;

    std::vector<Bone> _bones;
    std::vector<std::vector<BoneIndex>> _children;
    std::vector<Marker> _markers;
};

}  // namespace robot::world::anatomy
