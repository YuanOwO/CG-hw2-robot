#include "world/anatomy/builder.hpp"

#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "world/anatomy/parts/part.hpp"

namespace robot::world::anatomy {

RigBuilder::RigBuilder(const Vec3& rootOffset) : _rootOffset(rootOffset) {
    addBone(Bone("root", NO_BONE, Vec3(0.0f, 0.0f, 0.0f), {Joint(), std::nullopt, std::nullopt}));
}

void RigBuilder::addPart(Socket socket, Part& part) {
    if (socket.bone >= _rig._bones.size()) {
        throw std::invalid_argument("Socket bone index is out of range");
    }
    part.attachTo(*this, socket);
}

BoneIndex RigBuilder::addBone(Bone bone) {
    const BoneIndex index = _rig._bones.size();

    if (index == 0) {
        if (bone.parent() != NO_BONE) {
            throw std::invalid_argument("Root bone must not have a parent");
        }
    } else if (bone.parent() >= index) {
        throw std::invalid_argument("Parent bone index must be less than the current bone index");
    }

    const BoneIndex parent = bone.parent();

    _rig._bones.push_back(std::move(bone));
    _rig._children.emplace_back();

    if (index != 0) {
        _rig._children[parent].push_back(index);
    }

    return index;
}

void RigBuilder::addMarker(Marker marker) {
    if (marker.bone >= _rig._bones.size()) {
        throw std::invalid_argument("Marker bone index is out of range");
    }
    _rig._markers.push_back(std::move(marker));
}

Rig RigBuilder::build() && {
    // findBone 依名字查詢，名稱重複時只會找到第一根
    std::unordered_set<std::string_view> names;
    for (const Bone& bone : _rig._bones) {
        if (!names.insert(bone.name()).second) {
            throw std::invalid_argument("Duplicate bone name: " + bone.name());
        }
    }

    return std::move(_rig);
}

}  // namespace robot::world::anatomy
