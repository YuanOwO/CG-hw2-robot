#include "view/animate/robot_animator.hpp"

#include <algorithm>
#include <cmath>

#include "math/constants.hpp"
#include "math/utils.hpp"
#include "world/entity/robot.hpp"
#include "world/entity/robot_spec.hpp"

namespace robot::view::animate {

namespace {

// 擺動倍率為 1 時各關節的振幅，單位為度
constexpr float HIP_AMPLITUDE = 25.0f;
constexpr float KNEE_AMPLITUDE = 40.0f;
constexpr float SHOULDER_AMPLITUDE = 20.0f;
constexpr float ELBOW_AMPLITUDE = 15.0f;

constexpr float RAISED_SHOULDER_PITCH = 180.0f;
constexpr float RAISED_SHOULDER_ROLL = 20.0f;

/// 一個週期 (左右各一步) 走過的距離；一步是擺動倍率為 1 時兩腳張開的前後距離，這個速度下腳不會在地上滑
float cycleLength() {
    const float legLength = world::entity::Robot::body().legSpec.length();
    return 2.0f * (2.0f * legLength * std::sin(radians(HIP_AMPLITUDE)));
}

}  // namespace

world::anatomy::Pose RobotAnimator::animate(const world::entity::Entity& entity) const {
    const auto& body = world::entity::Robot::body();
    const auto& gait = entity.gait();

    world::anatomy::Pose pose = entity.pose();

    pose[body.head.head()].yaw = wrapSignedDegrees(entity.look().yaw - entity.yaw());
    pose[body.head.head()].pitch = entity.look().pitch;

    float phase = 2.0f * PI * gait.distance / cycleLength();
    float s = std::sin(phase);
    float c = std::cos(phase);

    // 左右腳反相；右腿的 pitch 沒有被 inverted，正值同樣是往前抬
    float hip = HIP_AMPLITUDE * s * gait.amount;
    pose[body.leftLeg.thigh()].pitch += hip;
    pose[body.rightLeg.thigh()].pitch -= hip;

    // 大腿往前擺 (相位導數為正) 的那隻腳才彎膝，讓腳尖離地
    pose[body.leftLeg.shin()].pitch -= KNEE_AMPLITUDE * std::max(0.0f, c) * gait.amount;
    pose[body.rightLeg.shin()].pitch -= KNEE_AMPLITUDE * std::max(0.0f, -c) * gait.amount;

    // 走路時手與同側腳反向擺動、手肘微彎；跑步時改成雙手伸直高舉過頭，左右張開成 V 字
    float shoulder = SHOULDER_AMPLITUDE * s * gait.amount;
    float raise_pitch = RAISED_SHOULDER_PITCH * gait.running;
    float raise_roll = RAISED_SHOULDER_ROLL * gait.running;
    float elbow = ELBOW_AMPLITUDE * gait.amount * (1.0f - gait.running);

    pose[body.leftArm.upperArm()].pitch += raise_pitch - shoulder;
    pose[body.rightArm.upperArm()].pitch += raise_pitch + shoulder;
    pose[body.leftArm.upperArm()].roll += raise_roll;
    pose[body.rightArm.upperArm()].roll -= raise_roll;
    pose[body.leftArm.forearm()].pitch += elbow;
    pose[body.rightArm.forearm()].pitch += elbow;

    // 支撐腳是直的，大腿斜 hip 度時髖部降低 L(1 - cos hip)，身體因此上下起伏
    float legLength = body.legSpec.length();
    pose.rootOffset().y -= legLength * (1.0f - std::cos(radians(hip)));

    return pose;
}

}  // namespace robot::view::animate
