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
#include "world/collision/collision.hpp"

namespace robot::world::anatomy {

struct NeckSpec {
    Vec3 size;
    JointLimits yaw, pitch, roll;

    std::optional<geometry::Mesh> neckMesh;
};

class Neck final : public Part {
   public:
    Neck(const std::string& name, const NeckSpec& spec) : Part(name), _neckSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("NeckSpec size must be positive");
        }
    }

    BoneIndex neck() const {
        requireAttached();
        return _neck;
    }

    Socket headSocket() const {
        requireAttached();
        return Socket(_neck, Vec3(0.0f, _neckSpec.size.y, 0.0f));
    }

   protected:
    void onAttach(RigBuilder& builder, Socket socket) override {
        collision::Box neckBox(Vec3(0.0f, _neckSpec.size.y * 0.5f, 0.0f), _neckSpec.size * 0.5f);
        BoneSpec neckBoneSpec{Joint(_neckSpec.yaw, _neckSpec.pitch, _neckSpec.roll), neckBox, _neckSpec.neckMesh};

        _neck = builder.addBone(Bone(name(), socket.bone, socket.offset, neckBoneSpec));
    }

   private:
    NeckSpec _neckSpec;

    BoneIndex _neck = NO_BONE;
};

}  // namespace robot::world::anatomy
