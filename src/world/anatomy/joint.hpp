#pragma once

#include <cstddef>

namespace robot::world::anatomy {

struct JointLimits {
    JointLimits() = default;
    JointLimits(float minAngle, float maxAngle, float maxSpeed);

    float minAngle = 0.0f;
    float maxAngle = 0.0f;
    float maxSpeed = 0.0f;

    bool locked() const;
    float clamp(float angle) const;
    JointLimits inverted() const;
};

class Joint {
   public:
    Joint() = default;
    Joint(const JointLimits& yaw, const JointLimits& pitch, const JointLimits& roll);
    ~Joint() = default;

    std::size_t dofCount() const { return _dofCount; }

    const JointLimits& yaw() const { return _yaw; }
    const JointLimits& pitch() const { return _pitch; }
    const JointLimits& roll() const { return _roll; }

   private:
    JointLimits _yaw;
    JointLimits _pitch;
    JointLimits _roll;
    std::size_t _dofCount = 0;
};

struct JointAngles {
    JointAngles() : yaw(0.0f), pitch(0.0f), roll(0.0f) {}
    JointAngles(float yaw, float pitch, float roll) : yaw(yaw), pitch(pitch), roll(roll) {}

    float yaw;
    float pitch;  ///< 度，繞 −x 軸：骨頭面向 +z，正值往上抬，與實體 pitch 的方向一致
    float roll;

    static JointAngles blend(const JointAngles& a, const JointAngles& b, float t);
};

}  // namespace robot::world::anatomy
