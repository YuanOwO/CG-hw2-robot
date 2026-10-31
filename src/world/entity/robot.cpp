#include "world/entity/robot.hpp"

#include <array>
#include <cstddef>
#include <utility>

#include "common/units.hpp"
#include "math/constants.hpp"
#include "world/anatomy/builder.hpp"
#include "world/anatomy/joint.hpp"

namespace robot::world::entity {

namespace {

const anatomy::BipedTorsoSpec ROBOT_TORSO_SPEC = {
    Vec3(8.0f, 9.0f, 4.0f) * PX,                 // size
    anatomy::JointLimits(-45.0f, 45.0f, 90.0f),  // pitch
    anatomy::JointLimits(),                      // roll
    std::nullopt,                                // torsoMesh
    Vec3(0.0f, 9.0f, 0.0f) * PX,                 // neckOffset
    9.0f * PX,                                   // shoulderHeight
    4.0f * PX,                                   // hipSpacing
};

const anatomy::NeckSpec ROBOT_NECK_SPEC = {
    Vec3(2.0f, 1.0f, 2.0f) * PX,                 // size
    anatomy::JointLimits(),                      // yaw
    anatomy::JointLimits(-45.0f, 45.0f, 90.0f),  // pitch
    anatomy::JointLimits(),                      // roll
    std::nullopt,                                // neckMesh
};

const anatomy::HeadSpec ROBOT_HEAD_SPEC = {
    Vec3(5.0f, 5.0f, 5.0f) * PX,                 // size
    anatomy::JointLimits(-70.0f, 70.0f, 90.0f),  // yaw
    anatomy::JointLimits(-60.0f, 50.0f, 90.0f),  // pitch
    std::nullopt,                                // headMesh
};

const anatomy::ArmSpec ROBOT_ARM_SPEC = {
    Vec3(3.0f, 5.0f, 3.0f) * PX,                  // upperArmSize
    anatomy::JointLimits(),                       // shoulderYaw
    anatomy::JointLimits(-60.0f, 170.0f, 90.0f),  // shoulderPitch
    anatomy::JointLimits(0.0f, 170.0f, 90.0f),    // shoulderRoll
    std::nullopt,                                 // upperArmMesh
    Vec3(3.0f, 6.0f, 3.0f) * PX,                  // forearmSize
    anatomy::JointLimits(0.0f, 145.0f, 90.0f),    // elbowPitch
    std::nullopt,                                 // forearmMesh
};

const anatomy::LegSpec ROBOT_LEG_SPEC = {
    Vec3(4.0f, 6.0f, 4.0f) * PX,                  // thighSize
    anatomy::JointLimits(0.0f, 45.0f, 90.0f),     // hipYaw
    anatomy::JointLimits(-30.0f, 120.0f, 90.0f),  // hipPitch
    anatomy::JointLimits(),                       // hipRoll
    std::nullopt,                                 // thighMesh
    Vec3(4.0f, 7.0f, 4.0f) * PX,                  // shinSize
    anatomy::JointLimits(-140.0f, 0.0f, 90.0f),   // kneePitch
    std::nullopt,                                 // shinMesh
};

const RobotBody ROBOT_BODY(ROBOT_TORSO_SPEC, ROBOT_NECK_SPEC, ROBOT_HEAD_SPEC, ROBOT_ARM_SPEC, ROBOT_LEG_SPEC);

// 機器人的相關物理參數，單位為國際單位制
constexpr float ROBOT_MASS = 70.0f;    // kg
constexpr float ROBOT_HEIGHT = 1.75f;  // m

constexpr float ROBOT_JUMP_HEIGHT = 0.5f;  // m
constexpr float ROBOT_SNEAK_SPEED = 0.6f;  // m/s
constexpr float ROBOT_WALK_SPEED = 1.2f;   // m/s
constexpr float ROBOT_RUN_SPEED = 10.0f;   // m/s

const std::array<collision::UprightCylinder, anatomy::STANCE_COUNT> COLLIDERS = {
    collision::UprightCylinder(Vec3(), 0.44743190822739f, 1.75f),             // Standing
    collision::UprightCylinder(Vec3(), 0.57622152858081f, 1.5657849302036f),  // Sneaking
    collision::UprightCylinder(Vec3(), 0.87766592186389f, 1.0625f),           // Sitting
};

}  // namespace

#pragma region RobotBody

RobotBody::RobotBody(const anatomy::BipedTorsoSpec& torsoSpec, const anatomy::NeckSpec& neckSpec,
                     const anatomy::HeadSpec& headSpec, const anatomy::ArmSpec& armSpec,
                     const anatomy::LegSpec& legSpec)
    : _torsoSpec(torsoSpec),
      _neckSpec(neckSpec),
      _headSpec(headSpec),
      _armSpec(armSpec),
      _legSpec(legSpec),
      torso("torso", torsoSpec),
      neck("neck", neckSpec),
      head("head", headSpec),
      leftArm("left_arm", anatomy::Side::Left, armSpec),
      rightArm("right_arm", anatomy::Side::Right, armSpec),
      leftLeg("left_leg", anatomy::Side::Left, legSpec),
      rightLeg("right_leg", anatomy::Side::Right, legSpec),
      skeleton(assemble()),
      defaultPoses({standingPose(), sneakingPose(), sittingPose()}) {}

anatomy::Skeleton RobotBody::assemble() {
    anatomy::SkeletonBuilder builder;

    builder.addPart(builder.root(), torso);
    builder.addPart(torso.neck(), neck);
    builder.addPart(neck.headSocket(), head);
    builder.addPart(torso.shoulder(anatomy::Side::Left), leftArm);
    builder.addPart(torso.shoulder(anatomy::Side::Right), rightArm);
    builder.addPart(torso.hip(anatomy::Side::Left), leftLeg);
    builder.addPart(torso.hip(anatomy::Side::Right), rightLeg);

    return std::move(builder).build();
}

anatomy::Pose RobotBody::standingPose() const {
    anatomy::Pose pose(skeleton.boneCount());
    pose.rootOffset().y = _legSpec.length();

    return pose;
}

anatomy::Pose RobotBody::sneakingPose() const {
    anatomy::Pose pose(skeleton.boneCount());
    pose.rootOffset().y = _legSpec.length() * SQRT_3 / 2.0f;

    pose[torso.torso()] = anatomy::JointAngles(0.0f, -30.0f, 0.0f);
    pose[neck.neck()] = anatomy::JointAngles(0.0f, 30.0f, 0.0f);

    pose[leftLeg.thigh()] = anatomy::JointAngles(0.0f, 60.0f, 0.0f);
    pose[rightLeg.thigh()] = anatomy::JointAngles(0.0f, 60.0f, 0.0f);
    pose[leftLeg.shin()] = anatomy::JointAngles(0.0f, -60.0f, 0.0f);
    pose[rightLeg.shin()] = anatomy::JointAngles(0.0f, -60.0f, 0.0f);

    return pose;
}

anatomy::Pose RobotBody::sittingPose() const {
    anatomy::Pose pose(skeleton.boneCount());
    pose.rootOffset().y = _legSpec.thighSize.z * 0.5f;

    pose[leftLeg.thigh()] = anatomy::JointAngles(15.0f, 90.0f, 0.0f);
    pose[rightLeg.thigh()] = anatomy::JointAngles(-15.0f, 90.0f, 0.0f);

    return pose;
}

#pragma endregion RobotBody

#pragma region Robot

Robot::Robot(EntityId id, Seed seed, Vec3 position, std::unique_ptr<brain::Brain> brain)
    : Entity(id, seed, ROBOT_MASS, position, &ROBOT_BODY.skeleton, std::move(brain)) {}

collision::UprightCylinder Robot::localCollider(anatomy::Stance stance) const {
    return COLLIDERS.at(static_cast<std::size_t>(stance));
}

float Robot::jumpHeight() const {
    return ROBOT_JUMP_HEIGHT;
}

float Robot::sneakSpeed() const {
    return ROBOT_SNEAK_SPEED;
}

float Robot::walkSpeed() const {
    return ROBOT_WALK_SPEED;
}

float Robot::runSpeed() const {
    return ROBOT_RUN_SPEED;
}

bool Robot::supportsStance(anatomy::Stance stance) const {
    return stance != anatomy::Stance::Count;
}

void Robot::updatePose(anatomy::Pose& pose) const {
    const auto& poses = ROBOT_BODY.defaultPoses;
    const std::size_t& from = static_cast<std::size_t>(stance().from);
    const std::size_t& to = static_cast<std::size_t>(stance().to);
    pose = anatomy::Pose::blend(poses.at(from), poses.at(to), stance().progress);
}

#pragma endregion Robot

}  // namespace robot::world::entity
