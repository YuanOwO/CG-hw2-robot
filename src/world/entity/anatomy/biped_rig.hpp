#pragma once

#include "world/entity/anatomy/rig.hpp"

namespace robot::world::entity::anatomy {

struct BipedSize {
    Vec3 body;
    Vec3 head;
    Vec3 upperLeg;  // 從髖關節到膝關節
    Vec3 lowerLeg;  // 從膝關節到腳底
};

struct BipedGait {
    float strideLength;  // 公尺，走這麼遠腿擺一個來回
    float swing;         // 度，最大擺幅
    float kneeBend;      // 度，腿擺到最後方時膝蓋彎曲的角度
};

class BipedRig : public Rig {
   public:
    BipedRig(const BipedSize& size, const BipedGait& gait);

   protected:
    BoneIndex torso() const { return _torso; }

   private:
    BoneIndex _torso;
};

}  // namespace robot::world::entity::anatomy
