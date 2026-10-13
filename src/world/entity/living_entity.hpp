#pragma once

#include <cstdint>
#include <vector>

#include "math/vec3.hpp"
#include "world/entity/anatomy/rig.hpp"
#include "world/entity/entity.hpp"

namespace robot::world::entity {

// 相對於 yaw 的移動輸入
struct MoveInput {
    float forward = 0.0f;  // -1～1，正值往前
    float strafe = 0.0f;   // -1～1，正值往右
};

class LivingEntity : public Entity {
   public:
    // rig 由同種實體共用，生命週期必須比所有實體長；walkSpeed 為公尺 / 秒
    LivingEntity(EntityId id, std::uint64_t seed, const anatomy::Rig& rig, const Vec3& position, float yaw,
                 float walkSpeed);

    void tick(float dt, const World& world) final;

    const anatomy::Rig& rig() const { return _rig; }
    float bodyYaw() const { return _bodyYaw; }  // 身體的朝向，與 yaw 的夾角不超過 rig.maxHeadYaw()
    float walkSpeed() const { return _walkSpeed; }
    float speed() const { return _speed; }  // 這個 tick 實際的速度

    anatomy::AnimationInput animationInput() const;
    Mat4 rootTransform() const;         // 原點在腳底，朝向 bodyYaw
    // 依目前的姿勢算出每根骨頭的世界座標變換矩陣，依骨頭編號輸出到 out
    void computeBoneTransforms(std::vector<Mat4>& out) const;
    Vec3 eyePosition() const override;  // 依目前的姿勢現算；沒有眼睛時退回 Entity::eyePosition

   protected:
    virtual MoveInput aiStep(float dt, const World& world) = 0;

   private:
    const anatomy::Rig& _rig;
    float _walkSpeed;

    float _speed = 0.0f;
    float _bodyYaw;               // 度，[0, 360)
    float _walkDistance = 0.0f;   // 累計走過的距離，決定步伐的相位：走多遠腳就擺多少，不會滑步
    float _walkAnimation = 0.0f;  // [0, 1]，速度會瞬間變化，這個值則慢慢跟上，停下時腿才不會瞬間歸位

    void travel(const MoveInput& input, float dt);
    void turnBody(const Vec3& direction, float dt);
};

}  // namespace robot::world::entity
