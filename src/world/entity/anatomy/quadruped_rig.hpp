#pragma once

#include "world/entity/anatomy/rig.hpp"

namespace robot::world::entity::anatomy {

struct QuadrupedSize {
    Vec3 body;
    Vec3 head;
    Vec3 upperLeg;  // 從髖關節到膝關節
    Vec3 lowerLeg;  // 從膝關節到腳底；零向量表示只有一節
};

struct QuadrupedGait {
    float strideLength;   // 公尺，走這麼遠腿擺一個來回
    float swing;          // 度，最大擺幅
    float frontKneeBend;  // 度，前腳擺到最後方時膝蓋往後彎的角度
    float backKneeBend;   // 度，後腳擺到最後方時膝蓋往前彎的角度
};

enum class LegSlot { FrontLeft, FrontRight, BackLeft, BackRight };

// 四足的共同部分：軀幹、四條腿的相位與膝蓋。子類別決定頭與腿的第一節接在哪
class QuadrupedRig : public Rig {
   protected:
    static constexpr LegSlot LEG_SLOTS[] = {LegSlot::FrontLeft, LegSlot::FrontRight, LegSlot::BackLeft,
                                            LegSlot::BackRight};

    // 建立軀幹：轉軸在底面中心，高度等於腿長
    QuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait);

    const QuadrupedSize& size() const { return _size; }
    BoneIndex torso() const { return _torso; }

    static float sideOf(LegSlot slot);   // +1 為左（+x）
    static float frontOf(LegSlot slot);  // +1 為前（+z）

    // upper 的名稱由這裡依 slot 填入；第二節接在 upper 的末端，往下長
    void addLeg(LegSlot slot, BoneShape upper);

   private:
    QuadrupedSize _size;
    QuadrupedGait _gait;
    BoneIndex _torso;
};

}  // namespace robot::world::entity::anatomy
