#pragma once

#include "world/entity/anatomy/rig.hpp"

namespace robot::world::entity::anatomy {

struct BipedSize {
    Vec3 body;
    Vec3 head;
    Vec3 upperLeg;  // 從髖關節到膝關節
    Vec3 lowerLeg;  // 從膝關節到腳底
    float hipWidth;  // 兩條腿中心的距離
};

// 人的膝蓋往後彎；鳥露在外面的關節其實是腳踝，往前彎
enum class KneeDirection { Backward, Forward };

struct BipedGait {
    float strideLength;  // 公尺，走這麼遠腿擺一個來回
    float swing;         // 度，最大擺幅
    float kneeBend;      // 度，腿擺到最後方時膝蓋彎曲的角度
    KneeDirection knee = KneeDirection::Backward;
};

// 二足的共同部分：軀幹與兩條腿。子類別決定頭與其他部位接在哪
class BipedRig : public Rig {
   protected:
    // 建立軀幹與腿：軀幹的轉軸在底面中心，高度等於腿長；兩條腿並排在軀幹正下方
    BipedRig(const BipedSize& size, const BipedGait& gait);

    BoneIndex torso() const { return _torso; }

   private:
    BoneIndex _torso;
};

}  // namespace robot::world::entity::anatomy
