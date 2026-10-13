#pragma once

#include <cstdint>

#include "math/vec3.hpp"
#include "world/entity/anatomy/rig.hpp"
#include "world/entity/living_entity.hpp"

namespace robot::world::entity {

class Mob : public LivingEntity {
   public:
    // maxTurnRate 為度 / 秒
    Mob(EntityId id, std::uint64_t seed, const anatomy::Rig& rig, const Vec3& position, float yaw, float walkSpeed,
        float maxTurnRate);

   protected:
    virtual void think(float dt, const World& world) = 0;

    // speedFactor 為 walkSpeed 的倍率，夾在 [0, 1]；0 表示停下
    void steerToward(float targetYaw, float speedFactor);
    float targetYaw() const { return _targetYaw; }

   private:
    float _maxTurnRate;
    float _targetYaw;
    float _speedFactor = 0.0f;

    MoveInput aiStep(float dt, const World& world) final;  // think，再轉向並換算成移動輸入
};

}  // namespace robot::world::entity
