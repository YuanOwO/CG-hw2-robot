
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
    std::optional<geometry::Mesh> torsoMesh;
    Vec3 neckOffset;
    float shoulderHeight;
    float hipSpacing;
};

class BipedTorso final : public Part {
   public:
    BipedTorso(const std::string& name, const BipedTorsoSpec& spec) : Part(name), _torsoSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("BipedTorsoSpec size must be positive");
        }

        if ((spec.neckOffset.x < spec.size.x * -0.5f || spec.neckOffset.x > spec.size.x * 0.5f) ||
            (spec.neckOffset.y < 0.0f || spec.neckOffset.y > spec.size.y) ||
            (spec.neckOffset.z < spec.size.z * -0.5f || spec.neckOffset.z > spec.size.z * 0.5f)) {
            throw std::invalid_argument("BipedTorsoSpec neckOffset must be within torso bounds and above torso base");
        }

        if (spec.shoulderHeight < 0.0f || spec.shoulderHeight > spec.size.y) {
            throw std::invalid_argument("BipedTorsoSpec shoulderHeight must be within torso height");
        }

        if (spec.hipSpacing < 0.0f || spec.hipSpacing > spec.size.x) {
            throw std::invalid_argument("BipedTorsoSpec hipSpacing must be within torso width");
        }
    }

    BoneIndex torso() const {
        requireAttached();
        return _torso;
    }

    Socket neck() const {
        requireAttached();
        return Socket(_torso, _torsoSpec.neckOffset);
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
    void onAttach(SkeletonBuilder& builder, Socket socket) override {
        collision::UprightBox torsoBox(Vec3(), _torsoSpec.size, 0.0f);
        BoneSpec torsoBoneSpec{Joint(), torsoBox, _torsoSpec.torsoMesh};

        _torso = builder.addBone(Bone(name(), socket.bone, socket.offset, torsoBoneSpec));
    }

   private:
    BipedTorsoSpec _torsoSpec;

    BoneIndex _torso = NO_BONE;
};

struct QuadrupedTorsoSpec {
    Vec3 size;
    std::optional<geometry::Mesh> torsoMesh;
    Vec3 neckOffset;
    float shoulderSpacing;
    float hipSpacing;
    float shoulderZ;
    float hipZ;
};

class QuadrupedTorso final : public Part {
   public:
    QuadrupedTorso(const std::string& name, const QuadrupedTorsoSpec& spec) : Part(name), _torsoSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("QuadrupedTorsoSpec size must be positive");
        }

        if ((spec.neckOffset.x < spec.size.x * -0.5f || spec.neckOffset.x > spec.size.x * 0.5f) ||
            (spec.neckOffset.y < 0.0f || spec.neckOffset.y > spec.size.y) ||
            (spec.neckOffset.z < spec.size.z * -0.5f || spec.neckOffset.z > spec.size.z * 0.5f)) {
            throw std::invalid_argument(
                "QuadrupedTorsoSpec neckOffset must be within torso bounds and above torso base");
        }

        if (spec.shoulderSpacing <= 0.0f || spec.shoulderSpacing >= spec.size.x) {
            throw std::invalid_argument(
                "QuadrupedTorsoSpec shoulderSpacing must be positive and less than torso width");
        }

        if (spec.hipSpacing <= 0.0f || spec.hipSpacing >= spec.size.x) {
            throw std::invalid_argument("QuadrupedTorsoSpec hipSpacing must be positive and less than torso width");
        }

        if (spec.shoulderZ < spec.size.z * -0.5f || spec.shoulderZ > spec.size.z * 0.5f) {
            throw std::invalid_argument("QuadrupedTorsoSpec shoulderZ must be within torso depth");
        }

        if (spec.hipZ < spec.size.z * -0.5f || spec.hipZ > spec.size.z * 0.5f) {
            throw std::invalid_argument("QuadrupedTorsoSpec hipZ must be within torso depth");
        }
    }

    BoneIndex torso() const {
        requireAttached();
        return _torso;
    }

    Socket neck() const {
        requireAttached();
        return Socket(_torso, _torsoSpec.neckOffset);
    }

    Socket shoulder(Side side) const {
        requireAttached();
        Vec3 offset(_torsoSpec.shoulderSpacing * 0.5f * sideSign(side), 0.0f, _torsoSpec.shoulderZ);
        return Socket(_torso, offset);
    }

    Socket hip(Side side) const {
        requireAttached();
        Vec3 offset(_torsoSpec.hipSpacing * 0.5f * sideSign(side), 0.0f, _torsoSpec.hipZ);
        return Socket(_torso, offset);
    }

   protected:
    void onAttach(SkeletonBuilder& builder, Socket socket) override {
        collision::UprightBox torsoBox(Vec3(), _torsoSpec.size, 0.0f);
        BoneSpec torsoBoneSpec{Joint(), torsoBox, _torsoSpec.torsoMesh};

        _torso = builder.addBone(Bone(name(), socket.bone, socket.offset, torsoBoneSpec));
    }

   private:
    QuadrupedTorsoSpec _torsoSpec;

    BoneIndex _torso = NO_BONE;
};

}  // namespace robot::world::anatomy
