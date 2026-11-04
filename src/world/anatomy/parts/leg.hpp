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

struct LegSpec {
    Vec3 thighSize;
    JointLimits hipYaw, hipPitch, hipRoll;
    std::optional<geometry::Mesh> thighMesh;

    std::optional<Vec3> shinSize;
    std::optional<JointLimits> kneePitch;
    std::optional<geometry::Mesh> shinMesh;

    float length() const { return thighSize.y + (shinSize.has_value() ? shinSize->y : 0.0f); }
};

class Leg final : public Part {
   public:
    Leg(const std::string& name, Side side, const LegSpec& spec) : Part(name), _legSpec(spec) {
        if (spec.thighSize.x <= 0.0f || spec.thighSize.y <= 0.0f || spec.thighSize.z <= 0.0f) {
            throw std::invalid_argument("LegSpec thighSize must be positive");
        }

        if (spec.shinSize.has_value() &&
            (spec.shinSize->x <= 0.0f || spec.shinSize->y <= 0.0f || spec.shinSize->z <= 0.0f)) {
            throw std::invalid_argument("LegSpec shinSize must be positive");
        }

        if (_legSpec.shinSize.has_value() != _legSpec.kneePitch.has_value()) {
            throw std::invalid_argument("LegSpec shinSize and kneePitch must be both present or both absent");
        }

        // 傳入的關節限制是針對左腿的，右腿的限制需要反向
        if (side == Side::Right) {
            _legSpec.hipYaw = _legSpec.hipYaw.inverted();
            _legSpec.hipRoll = _legSpec.hipRoll.inverted();
        }
    }

    BoneIndex thigh() const {
        requireAttached();
        return _thigh;
    }

    BoneIndex shin() const {
        requireAttached();
        return _shin;
    }

    Socket ankle() const {
        requireAttached();
        if (_shin == NO_BONE) {
            return Socket(_thigh, Vec3(0.0f, -_legSpec.thighSize.y, 0.0f));
        }
        return Socket(_shin, Vec3(0.0f, -_legSpec.shinSize->y, 0.0f));
    }

   protected:
    void onAttach(SkeletonBuilder& builder, Socket socket) override {
        collision::UprightBox thighBox(Vec3(0.0f, -_legSpec.thighSize.y, 0.0f), _legSpec.thighSize, 0.0f);
        BoneSpec thighSpec{Joint(_legSpec.hipYaw, _legSpec.hipPitch, _legSpec.hipRoll), thighBox, _legSpec.thighMesh};

        _thigh = builder.addBone(Bone(name() + "_thigh", socket.bone, socket.offset, thighSpec));

        if (!_legSpec.shinSize.has_value()) {
            return;
        }

        Vec3 kneeOffset(0.0f, -_legSpec.thighSize.y, 0.0f);
        collision::UprightBox shinBox(Vec3(0.0f, -_legSpec.shinSize->y, 0.0f), _legSpec.shinSize.value(), 0.0f);
        BoneSpec shinSpec{Joint(JointLimits(), _legSpec.kneePitch.value(), JointLimits()), shinBox, _legSpec.shinMesh};

        _shin = builder.addBone(Bone(name() + "_shin", _thigh, kneeOffset, shinSpec));
    }

   private:
    LegSpec _legSpec;

    BoneIndex _thigh = NO_BONE;
    BoneIndex _shin = NO_BONE;
};

}  // namespace robot::world::anatomy
