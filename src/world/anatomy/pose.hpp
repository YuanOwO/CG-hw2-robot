#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "math/utils.hpp"
#include "world/anatomy/joint.hpp"
#include "world/anatomy/types.hpp"

namespace robot::world::anatomy {

class Pose {
   public:
    explicit Pose(const std::size_t boneCount) : _joints(boneCount) {}
    ~Pose() = default;

    std::size_t boneCount() const { return _joints.size(); }

    Vec3& rootOffset() { return _rootOffset; }
    const Vec3& rootOffset() const { return _rootOffset; }

    JointAngles& operator[](const BoneIndex index) { return _joints.at(index); }
    const JointAngles& operator[](const BoneIndex index) const { return _joints.at(index); }

    /// t in [0, 1]，0 表示完全使用 a，1 表示完全使用 b
    static Pose blend(const Pose& a, const Pose& b, float t) {
        if (a.boneCount() != b.boneCount()) {
            throw std::invalid_argument("Cannot blend poses with different bone counts");
        }

        Pose result(a.boneCount());
        result.rootOffset() = lerp(a.rootOffset(), b.rootOffset(), t);
        for (std::size_t i = 0; i < a.boneCount(); ++i) {
            result[i] = JointAngles::blend(a[i], b[i], t);
        }
        return result;
    }

   private:
    Vec3 _rootOffset;
    std::vector<JointAngles> _joints;
};

}  // namespace robot::world::anatomy
