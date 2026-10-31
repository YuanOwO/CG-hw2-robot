#pragma once

#include "math/vec3.hpp"
#include "world/anatomy/bone.hpp"
#include "world/anatomy/skeleton.hpp"
#include "world/anatomy/types.hpp"

namespace robot::world::anatomy {

class Part;

class SkeletonBuilder {
   public:
    SkeletonBuilder(const Vec3& rootOffset = Vec3(0.0f, 0.0f, 0.0f));
    ~SkeletonBuilder() = default;

    Socket root() const { return Socket{0, _rootOffset}; }

    void addPart(Socket socket, Part& part);

    BoneIndex addBone(Bone bone);
    void addMarker(Marker marker);

    Skeleton build() &&;

   private:
    Skeleton _skeleton;
    Vec3 _rootOffset;
};

}  // namespace robot::world::anatomy
