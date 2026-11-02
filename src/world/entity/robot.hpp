#pragma once

#include <array>
#include <memory>

#include "world/anatomy/parts/arm.hpp"
#include "world/anatomy/parts/head.hpp"
#include "world/anatomy/parts/leg.hpp"
#include "world/anatomy/parts/neck.hpp"
#include "world/anatomy/parts/torso.hpp"
#include "world/entity/entity.hpp"
#include "world/types.hpp"

namespace robot::world::entity {

struct RobotBody {
    anatomy::BipedTorsoSpec torsoSpec;
    anatomy::NeckSpec neckSpec;
    anatomy::HeadSpec headSpec;
    anatomy::ArmSpec armSpec;
    anatomy::LegSpec legSpec;

    anatomy::BipedTorso torso;
    anatomy::Neck neck;
    anatomy::Head head;
    anatomy::Arm leftArm;
    anatomy::Arm rightArm;
    anatomy::Leg leftLeg;
    anatomy::Leg rightLeg;
    anatomy::Skeleton skeleton;
    std::array<anatomy::Pose, static_cast<std::size_t>(anatomy::Stance::Count)> defaultPoses;

    RobotBody(const anatomy::BipedTorsoSpec& torsoSpec, const anatomy::NeckSpec& neckSpec,
              const anatomy::HeadSpec& headSpec, const anatomy::ArmSpec& armSpec, const anatomy::LegSpec& legSpec);

   private:
    anatomy::Skeleton assemble();

    anatomy::Pose standingPose() const;
    anatomy::Pose sneakingPose() const;
    anatomy::Pose sittingPose() const;
};

class Robot final : public Entity {
   public:
    Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain);

    static const RobotBody& body();

   protected:
    float jumpHeight() const override;
    float sneakSpeed() const override;
    float walkSpeed() const override;
    float runSpeed() const override;

    anatomy::MarkerIndex eyeMarker() const override;

    collision::UprightCylinder localCollider(anatomy::Stance stance) const override;
    bool supportsStance(anatomy::Stance stance) const override;

    void updatePose(anatomy::Pose& pose) const override;
};

}  // namespace robot::world::entity
