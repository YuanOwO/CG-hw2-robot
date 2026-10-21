#include "world/anatomy/joint.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include "math/constants.hpp"
#include "math/utils.hpp"

namespace robot::world::anatomy {
namespace {

void validateJointLimits(const JointLimits& limits, const std::string& name) {
    if (limits.maxAngle < limits.minAngle) {
        throw std::invalid_argument(name + " joint limits are invalid");
    }
    if (limits.maxSpeed < 0.0f) {
        throw std::invalid_argument(name + " joint max speed is invalid");
    }
}

}  // namespace

JointLimits::JointLimits(float minAngle, float maxAngle, float maxSpeed)
    : minAngle(minAngle), maxAngle(maxAngle), maxSpeed(maxSpeed) {
    validateJointLimits(*this, "Joint");
}

bool JointLimits::locked() const {
    return std::abs(maxAngle - minAngle) <= EPSILON;
}

float JointLimits::clamp(float angle) const {
    return std::clamp(angle, minAngle, maxAngle);
}

JointLimits JointLimits::inverted() const {
    return JointLimits(-maxAngle, -minAngle, maxSpeed);
}

Joint::Joint(const JointLimits& yaw, const JointLimits& pitch, const JointLimits& roll)
    : _yaw(yaw), _pitch(pitch), _roll(roll) {
    _dofCount = 0;

    validateJointLimits(yaw, "Yaw");
    if (!yaw.locked()) {
        _dofCount++;
    }

    validateJointLimits(pitch, "Pitch");
    if (!pitch.locked()) {
        _dofCount++;
    }

    validateJointLimits(roll, "Roll");
    if (!roll.locked()) {
        _dofCount++;
    }
}

JointAngles JointAngles::blend(const JointAngles& a, const JointAngles& b, float t) {
    return JointAngles{
        lerp(a.yaw, b.yaw, t),
        lerp(a.pitch, b.pitch, t),
        lerp(a.roll, b.roll, t),
    };
}

}  // namespace robot::world::anatomy
