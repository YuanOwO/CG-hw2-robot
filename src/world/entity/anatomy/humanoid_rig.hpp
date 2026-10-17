#pragma once

#include "world/entity/anatomy/biped_rig.hpp"

namespace robot::world::entity::anatomy {

struct HumanoidSize : public BipedSize {
    Vec3 upperArm;  // 從肩關節到肘關節
    Vec3 lowerArm;  // 從肘關節到手腕
    Vec3 hand;      // 從手腕到指尖；零向量表示沒有手掌
};

struct HumanoidGait : public BipedGait {
    float armSwing;   // 度，手的最大擺幅
    float elbowRest;  // 度，站著時手肘彎曲的角度
    float elbowBend;  // 度，手擺到最前方時手肘多彎的角度
};

class HumanoidRig : public BipedRig {
   public:
    HumanoidRig(const HumanoidSize& size, const HumanoidGait& gait);
};

}  // namespace robot::world::entity::anatomy
