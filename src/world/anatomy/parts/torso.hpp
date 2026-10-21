
#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include "geometry/mesh/mesh.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/bone.hpp"
#include "world/anatomy/builder.hpp"
#include "world/anatomy/parts/part.hpp"
#include "world/anatomy/types.hpp"

namespace robot::world::anatomy {

struct BipedTorsoSpec {
    Vec3 size;  ///< 寬、高、厚
    float shoulderHeight;
    float hipSpacing;

    std::optional<geometry::Mesh> torsoMesh;
};

class BipedTorso final : public Part {
   public:
    BipedTorso(const std::string& name, const BipedTorsoSpec& spec) : Part(name), _torsoSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("BipedTorsoSpec size must be positive");
        }

        if (spec.shoulderHeight <= 0.0f || spec.shoulderHeight >= spec.size.y) {
            throw std::invalid_argument("BipedTorsoSpec shoulderHeight must be positive and less than torso height");
        }

        if (spec.hipSpacing <= 0.0f || spec.hipSpacing >= spec.size.x) {
            throw std::invalid_argument("BipedTorsoSpec hipSpacing must be positive and less than torso width");
        }
    }

    BoneIndex torso() const {
        requireAttached();
        return _torso;
    }

    Socket neck() const {
        requireAttached();
        return Socket(_torso, Vec3(0.0f, _torsoSpec.size.y, 0.0f));
    }

    Socket shoulder(Side side) const {
        requireAttached();
        Vec3 offset(_torsoSpec.size.x * 0.5f * sideSign(side), _torsoSpec.shoulderHeight, 0.0f);
        return Socket(_torso, offset);
    }

    Socket hip(Side side) const {
        requireAttached();
        Vec3 offset(_torsoSpec.hipSpacing * 0.5f * sideSign(side), 0.0f, 0.0f);
        return Socket(_torso, offset);
    }

   protected:
    void onAttach(RigBuilder& builder, Socket socket) override {
        collision::Box torsoBox(Vec3(0.0f, _torsoSpec.size.y * 0.5f, 0.0f), _torsoSpec.size * 0.5f);
        BoneSpec torsoBoneSpec{Joint(), torsoBox, _torsoSpec.torsoMesh};

        _torso = builder.addBone(Bone(name(), socket.bone, socket.offset, torsoBoneSpec));
    }

   private:
    BipedTorsoSpec _torsoSpec;

    BoneIndex _torso = NO_BONE;
};

}  // namespace robot::world::anatomy
