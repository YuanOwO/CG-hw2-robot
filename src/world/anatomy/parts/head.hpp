#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include "geometry/mesh/mesh.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/bone.hpp"
#include "world/anatomy/builder.hpp"
#include "world/anatomy/joint.hpp"
#include "world/anatomy/parts/part.hpp"
#include "world/anatomy/types.hpp"
#include "world/collision/collider.hpp"

namespace robot::world::anatomy {

struct HeadSpec {
    Vec3 size;
    JointLimits yaw, pitch;
    std::optional<geometry::Mesh> headMesh;
};

class Head final : public Part {
   public:
    Head(const std::string& name, const HeadSpec& spec) : Part(name), _headSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("HeadSpec size must be positive");
        }
    }

    BoneIndex head() const {
        requireAttached();
        return _head;
    }

    MarkerIndex eye() const {
        requireAttached();
        return _eye;
    }

   protected:
    void onAttach(SkeletonBuilder& builder, Socket socket) override {
        collision::UprightBox headBox(Vec3(), _headSpec.size, 0.0f);
        BoneSpec headBoneSpec{Joint(_headSpec.yaw, _headSpec.pitch, JointLimits()), headBox, _headSpec.headMesh};
        _head = builder.addBone(Bone(name(), socket.bone, socket.offset, headBoneSpec));
        _eye = builder.addMarker(Marker{name() + "_eye", _head, Vec3(0.0f, _headSpec.size.y * 0.5f, 0.0f)});
    }

   private:
    HeadSpec _headSpec;

    BoneIndex _head = NO_BONE;
    MarkerIndex _eye = NO_MARKER;
};

}  // namespace robot::world::anatomy
