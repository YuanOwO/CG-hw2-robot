#pragma once

#include "world/entity/entity.hpp"

namespace robot::world::entity {

// 會自己走動的實體。分成兩半：
// - 子類別的 aiStep 決定「想往哪走、走多快」，透過 setMoveIntent 交出意圖；
// - Mob::travel 負責「怎麼走過去」：以有限的角速度轉向，轉彎時減速，再依速度移動。
// 之後玩家控制的機器人也能沿用 travel，只是意圖改由輸入決定。
class Mob : public Entity {
   public:
    // walkSpeed 為公尺 / 秒，maxTurnRate 為度 / 秒。
    Mob(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float walkSpeed, float maxTurnRate);

    // 固定先想再走，子類別只交出意圖，轉向與速度限制因此一定會套用。
    void tick(float dt, const World& world) final;

    float walkSpeed() const { return _walkSpeed; }
    float speed() const { return _speed; }

    // 累計走過的距離，繪製端以它決定步伐的相位：走多遠腳就擺多少，不會滑步。
    float walkDistance() const { return _walkDistance; }

    // 走路的程度，[0, 1]。速度會瞬間變化，這個值則慢慢跟上，繪製端拿來決定擺幅，停下時腿才不會瞬間歸位。
    float walkAnimation() const { return _walkAnimation; }

   protected:
    virtual void aiStep(float dt, const World& world) = 0;

    // speedFactor 為 walkSpeed 的倍率，夾在 [0, 1]；0 表示停下。
    void setMoveIntent(float targetYaw, float speedFactor);
    float targetYaw() const { return _targetYaw; }

   private:
    float _walkSpeed;
    float _maxTurnRate;

    float _targetYaw;  // 想要面向的方向，以有限的角速度轉過去
    float _speedFactor = 0.0f;

    float _speed = 0.0f;  // 這個 tick 實際的速度，轉彎時會低於 walkSpeed × speedFactor
    float _walkDistance = 0.0f;
    float _walkAnimation = 0.0f;

    void travel(float dt);
};

}  // namespace robot::world::entity
