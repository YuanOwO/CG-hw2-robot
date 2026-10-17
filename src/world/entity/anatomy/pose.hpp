#pragma once

#include <array>
#include <vector>

#include "world/entity/anatomy/skeleton.hpp"

namespace robot::world::entity::anatomy {

// 一個實體各關節目前的角度。只能透過 setter 修改，且一律依骨架夾住，任何時候都是合法的姿態
class Pose {
   public:
    explicit Pose(const Skeleton& skeleton);  // 每個角度從最接近 0 的合法值開始
    ~Pose() = default;

    float angle(BoneIndex boneIndex, int dofIndex) const;
    void setAngle(BoneIndex boneIndex, int dofIndex, float degree);  // 夾在範圍內

   private:
    const Skeleton& _skeleton;
    std::vector<std::array<float, MAX_DOFS>> _angles;
};

}  // namespace robot::world::entity::anatomy
