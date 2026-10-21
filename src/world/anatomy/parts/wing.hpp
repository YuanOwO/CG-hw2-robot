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

struct WingSpec {
    Vec3 size;
    JointLimits shoulderRoll;
    std::optional<geometry::Mesh> wingMesh;
};

class Wing final : public Part {
   public:
    Wing(const std::string& name, Side side, const WingSpec& spec) : Part(name), _side(side), _wingSpec(spec) {
        if (spec.size.x <= 0.0f || spec.size.y <= 0.0f || spec.size.z <= 0.0f) {
            throw std::invalid_argument("WingSpec size must be positive");
        }

        // 傳入的關節限制是針對左翼的，右翼的限制需要反向
        if (side == Side::Right) {
            _wingSpec.shoulderRoll = _wingSpec.shoulderRoll.inverted();
        }
    }

    BoneIndex wing() const {
        requireAttached();
        return _wing;
    }

   protected:
    void onAttach(RigBuilder& builder, Socket socket) override {
        Vec3 shoulderOffset(_wingSpec.size.x * 0.5f * sideSign(_side), 0.0f, 0.0f);
        collision::Box wingBox(Vec3(0.0f, -_wingSpec.size.y * 0.5f, 0.0f), _wingSpec.size * 0.5f);
        BoneSpec wingSpec{Joint(JointLimits(), JointLimits(), _wingSpec.shoulderRoll), wingBox, _wingSpec.wingMesh};

        _wing = builder.addBone(Bone(name(), socket.bone, socket.offset + shoulderOffset, wingSpec));
    }

   private:
    Side _side;
    WingSpec _wingSpec;

    BoneIndex _wing = NO_BONE;
};

}  // namespace robot::world::anatomy
