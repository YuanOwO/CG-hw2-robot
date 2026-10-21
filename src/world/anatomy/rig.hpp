#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

#include "math/mat4.hpp"
#include "world/anatomy/bone.hpp"
#include "world/anatomy/pose.hpp"
#include "world/anatomy/stance.hpp"
#include "world/collision/collision.hpp"

namespace robot::world::anatomy {

class Rig {
    friend class RigBuilder;

   public:
    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;
    Rig(Rig&&) = default;
    Rig& operator=(Rig&&) = default;

    std::size_t boneCount() const { return _bones.size(); }
    const Bone& bone(BoneIndex index) const { return _bones.at(index); }
    const std::vector<BoneIndex>& children(BoneIndex index) const { return _children.at(index); }
    BoneIndex findBone(std::string_view name) const;

    std::size_t markerCount() const { return _markers.size(); }
    const Marker& marker(std::size_t index) const { return _markers.at(index); }

    // const Pose& stancePose(Stance stance) const;
    // const collision::UprightCylinder& stanceCollider(Stance stance) const;

    void clampPose(Pose& pose) const;

    std::vector<Mat4> transforms(Pose pose) const;

   private:
    Rig() = default;

    std::vector<Bone> _bones;
    std::vector<std::vector<BoneIndex>> _children;
    std::vector<Marker> _markers;

    // std::array<Pose, STANCE_COUNT> _stanceAngles;
    // std::array<collision::UprightCylinder, STANCE_COUNT> _stanceColliders;
};

}  // namespace robot::world::anatomy
