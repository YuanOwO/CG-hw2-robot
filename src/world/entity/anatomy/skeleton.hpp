#pragma once

#include <cstddef>
#include <vector>

#include "math/mat4.hpp"
#include "math/vec3.hpp"

namespace robot::world::entity::anatomy {

class Pose;

using BoneIndex = int;
using MarkerIndex = int;

const BoneIndex NO_BONE = -1;
const BoneIndex ROOT_BONE = 0;
const MarkerIndex NO_MARKER = -1;
const std::size_t MAX_DOFS = 3;  // 每個骨頭最多三個自由度，對應旋轉的 x、y、z 軸

struct Dof {
    const char* name;
    Vec3 axis;       // 旋轉軸，單位向量
    float min, max;  // 度，旋轉範圍
};

// 只描述形狀；接在哪、怎麼轉由組裝的人和部件決定
struct BoneShape {
    const char* name;
    Vec3 pivot;                 // 相對於父骨頭的旋轉中心
    Vec3 tail;                  // 相對於自己的末端位置
    Vec3 boundsMin, boundsMax;  // 這個部位相對於自己的方塊
};

struct Bone {
    BoneShape shape;
    BoneIndex parent;  // -1 為根骨頭
    std::vector<Dof> dofs;
};

struct Marker {
    const char* name;
    BoneIndex bone;  // 所屬骨頭
    Vec3 offset;     // 相對於骨頭的偏移
};

class Skeleton {
   public:
    Skeleton();
    ~Skeleton() = default;

    int boneCount() const { return _bones.size(); }
    int markerCount() const { return _markers.size(); }
    const Bone& bone(BoneIndex index) const;
    const Marker& marker(MarkerIndex index) const;
    BoneIndex findBone(const char* name) const;  // 找不到時丟出例外
    const Dof& dof(BoneIndex boneIndex, int dofIndex) const;

    BoneIndex addBone(const Bone& bone);
    MarkerIndex addMarker(const Marker& marker);

    // 計算每個骨頭的世界座標變換矩陣，輸出到 out
    void computeTransforms(const Pose& pose, const Mat4& root, std::vector<Mat4>& out) const;
    // 計算標記在世界座標下的位置
    Vec3 markerPosition(const std::vector<Mat4>& transforms, MarkerIndex marker) const;

   private:
    std::vector<Bone> _bones;
    std::vector<Marker> _markers;
};

}  // namespace robot::world::entity::anatomy
