#include "world/anatomy/builder.hpp"

#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "world/anatomy/parts/part.hpp"

namespace robot::world::anatomy {

SkeletonBuilder::SkeletonBuilder(const Vec3& rootOffset) : _rootOffset(rootOffset) {
    addBone(Bone("root", NO_BONE, Vec3(0.0f, 0.0f, 0.0f), {Joint(), std::nullopt, std::nullopt}));
}

void SkeletonBuilder::addPart(Socket socket, Part& part) {
    if (socket.bone >= _skeleton._bones.size()) {
        throw std::invalid_argument("Socket bone index is out of range");
    }
    part.attachTo(*this, socket);
}

BoneIndex SkeletonBuilder::addBone(Bone bone) {
    const BoneIndex index = _skeleton._bones.size();

    if (index == 0) {
        if (bone.parent() != NO_BONE) {
            throw std::invalid_argument("Root bone must not have a parent");
        }
    } else if (bone.parent() >= index) {
        throw std::invalid_argument("Parent bone index must be less than the current bone index");
    }

    const BoneIndex parent = bone.parent();

    _skeleton._bones.push_back(std::move(bone));
    _skeleton._children.emplace_back();

    if (index != 0) {
        _skeleton._children[parent].push_back(index);
    }

    return index;
}

MarkerIndex SkeletonBuilder::addMarker(Marker marker) {
    if (marker.bone >= _skeleton._bones.size()) {
        throw std::invalid_argument("Marker bone index is out of range");
    }
    _skeleton._markers.push_back(std::move(marker));
    return _skeleton._markers.size() - 1;
}

Skeleton SkeletonBuilder::build() && {
    // findBone 依名字查詢，名稱重複時只會找到第一根
    std::unordered_set<std::string_view> names;
    for (const Bone& bone : _skeleton._bones) {
        if (!names.insert(bone.name()).second) {
            throw std::invalid_argument("Duplicate bone name: " + bone.name());
        }
    }

    return std::move(_skeleton);
}

}  // namespace robot::world::anatomy
