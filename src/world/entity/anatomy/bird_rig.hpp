#pragma once

#include "world/entity/anatomy/biped_rig.hpp"

namespace robot::world::entity::anatomy {

struct BirdSize : public BipedSize {
    Vec3 wing;
    Vec3 neck;  // 頭底面中心的位置，相對於軀幹頂面的中心；頭可以沉進軀幹、凸出前端
};

struct BirdGait : public BipedGait {
    float wingFlap;  // 度，走路時翅膀往外張開的最大角度
};

// 軀幹水平，body.z 為身長；頭在 neck 指定的位置，翅膀貼在軀幹兩側
class BirdRig : public BipedRig {
   public:
    BirdRig(const BirdSize& size, const BirdGait& gait);
};

}  // namespace robot::world::entity::anatomy
