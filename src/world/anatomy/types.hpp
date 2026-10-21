#pragma once

#include <cstddef>
#include <string>

#include "math/vec3.hpp"

namespace robot::world::anatomy {

using BoneIndex = std::size_t;

constexpr BoneIndex NO_BONE = static_cast<BoneIndex>(-1);

struct Socket {
    Socket(BoneIndex bone, const Vec3& offset) : bone(bone), offset(offset) {}

    BoneIndex bone;
    Vec3 offset;
};

enum class Side { Center, Left, Right };

constexpr float sideSign(Side side) {
    return side == Side::Right ? -1.0f : 1.0f;
}

inline std::string sideName(Side side) {
    if (side == Side::Center) {
        return "center";
    } else if (side == Side::Left) {
        return "left";
    } else {
        return "right";
    }
}

}  // namespace robot::world::anatomy
