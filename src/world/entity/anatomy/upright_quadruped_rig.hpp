#pragma once

#include "world/entity/anatomy/rig.hpp"

namespace robot::world::entity::anatomy {

struct UprightQuadrupedSize {
    Vec3 body;
    Vec3 head;
    Vec3 upperLeg;  // 從髖關節到膝關節
    Vec3 lowerLeg;  // 從膝關節到腳底
};

struct QuadrupedGait {
    float strideLength;  // 公尺，走這麼遠腿擺一個來回
    float swing;         // 度，最大擺幅
};

class UprightQuadrupedRig : public Rig {
   public:
    UprightQuadrupedRig(const UprightQuadrupedSize& size, const QuadrupedGait& gait);
};

}  // namespace robot::world::entity::anatomy
