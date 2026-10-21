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

struct ArmSpec {
    Vec3 upperArmSize;
    JointLimits shoulderYaw, shoulderPitch, shoulderRoll;
    std::optional<geometry::Mesh> upperArmMesh;

    std::optional<Vec3> forearmSize;
    std::optional<JointLimits> elbowPitch;
    std::optional<geometry::Mesh> forearmMesh;
};

class Arm final : public Part {
   public:
    Arm(const std::string& name, Side side, const ArmSpec& spec) : Part(name), _side(side), _armSpec(spec) {
        if (spec.upperArmSize.x <= 0.0f || spec.upperArmSize.y <= 0.0f || spec.upperArmSize.z <= 0.0f) {
            throw std::invalid_argument("ArmSpec upperArmSize must be positive");
        }

        if (spec.forearmSize.has_value() &&
            (spec.forearmSize->x <= 0.0f || spec.forearmSize->y <= 0.0f || spec.forearmSize->z <= 0.0f)) {
            throw std::invalid_argument("ArmSpec forearmSize must be positive");
        }

        if (_armSpec.forearmSize.has_value() != _armSpec.elbowPitch.has_value()) {
            throw std::invalid_argument("ArmSpec forearmSize and elbowPitch must be both present or both absent");
        }

        // 傳入的關節限制是針對左臂的，右臂的限制需要反向
        if (side == Side::Right) {
            _armSpec.shoulderYaw = _armSpec.shoulderYaw.inverted();
            _armSpec.shoulderRoll = _armSpec.shoulderRoll.inverted();
        }
    }

    BoneIndex upperArm() const {
        requireAttached();
        return _upperArm;
    }

    BoneIndex forearm() const {
        requireAttached();
        return _forearm;
    }

    Socket wrist() const {
        requireAttached();
        if (_forearm == NO_BONE) {
            return Socket(_upperArm, Vec3(0.0f, -_armSpec.upperArmSize.y, 0.0f));
        }
        return Socket(_forearm, Vec3(0.0f, -_armSpec.forearmSize->y, 0.0f));
    }

   protected:
    void onAttach(RigBuilder& builder, Socket socket) override {
        Vec3 shoulderOffset(_armSpec.upperArmSize.x * 0.5f * sideSign(_side), 0.0f, 0.0f);
        collision::Box upperArmBox(Vec3(0.0f, -_armSpec.upperArmSize.y * 0.5f, 0.0f), _armSpec.upperArmSize * 0.5f);
        BoneSpec upperArmSpec{Joint(_armSpec.shoulderYaw, _armSpec.shoulderPitch, _armSpec.shoulderRoll), upperArmBox,
                              _armSpec.upperArmMesh};

        _upperArm =
            builder.addBone(Bone(name() + "_upper_arm", socket.bone, socket.offset + shoulderOffset, upperArmSpec));

        if (!_armSpec.forearmSize.has_value()) {
            return;
        }

        Vec3 elbowOffset(0.0f, -_armSpec.upperArmSize.y, 0.0f);
        collision::Box forearmBox(Vec3(0.0f, -_armSpec.forearmSize->y * 0.5f, 0.0f),
                                  _armSpec.forearmSize.value() * 0.5f);
        BoneSpec forearmSpec{Joint(JointLimits(), _armSpec.elbowPitch.value(), JointLimits()), forearmBox,
                             _armSpec.forearmMesh};

        _forearm = builder.addBone(Bone(name() + "_forearm", _upperArm, elbowOffset, forearmSpec));
    }

   private:
    Side _side;
    ArmSpec _armSpec;

    BoneIndex _upperArm = NO_BONE;
    BoneIndex _forearm = NO_BONE;
};

}  // namespace robot::world::anatomy
