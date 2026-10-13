#pragma once

#include "world/entity/anatomy/skeleton.hpp"

namespace robot::world::entity::anatomy {

// 用於控制身體動作的輸入
struct AnimationInput {
    float lookYaw = 0.0f;       // 度，頭相對於身體
    float lookPitch = 0.0f;     // 度，正值抬頭
    float walkDistance = 0.0f;  // 公尺，決定步伐的相位
    float walkAmount = 0.0f;    // [0, 1]，決定擺幅
    bool onGround = true;
};

// 關節三個軸能轉的範圍，度，對稱為 ±limit；0 表示這一軸轉不動。
// 部件建出的骨頭一律依序帶有 yaw、pitch、roll 三個 DOF，角度以 JointAxis 存取：
//   yaw   繞 +y，正值往左轉
//   pitch 繞 -x，正值讓前端往上：頭抬起、往下垂的肢體往前抬
//   roll  繞 +z，正值往右傾
struct JointLimits {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
};

enum JointAxis { YAW, PITCH, ROLL };

// 會動的部件：建構時把自己的骨頭加進骨架，之後依輸入轉動這些骨頭
class Part {
   public:
    virtual ~Part() = default;

    // 部件記著自己的骨頭編號，複製出來的部件會和原本的驅動同一組骨頭
    Part(const Part&) = delete;
    Part& operator=(const Part&) = delete;
    Part(Part&&) = delete;
    Part& operator=(Part&&) = delete;

    virtual void animate(const AnimationInput& input, Pose& pose) const = 0;

   protected:
    Part() = default;
};

#pragma region Head

class Head : public Part {
   public:
    static constexpr JointLimits DEFAULT_LIMITS = {45.0f, 90.0f, 0.0f};

    Head(Skeleton& skeleton, BoneIndex parent, const BoneShape& shape, const JointLimits& limits = DEFAULT_LIMITS);

    BoneIndex bone() const { return _bone; }
    const JointLimits& limits() const { return _limits; }

    void animate(const AnimationInput& input, Pose& pose) const override;

   private:
    BoneIndex _bone;
    JointLimits _limits;
};

#pragma endregion  // Head

#pragma region Torso

// 身體的主幹，頭和四肢接在它上面。目前不會動
class Torso : public Part {
   public:
    Torso(Skeleton& skeleton, BoneIndex parent, const BoneShape& shape, const JointLimits& limits = {});

    BoneIndex bone() const { return _bone; }

    void animate(const AnimationInput& /*input*/, Pose& /*pose*/) const override {}

   private:
    BoneIndex _bone;
};

#pragma endregion  // Torso

#pragma region Limb

// 一隻肢體怎麼擺。由身體往外依序是根關節（髖、肩）、第一節、彎曲關節（膝、肘）、第二節。
// 角度都是 pitch，正值往前
struct LimbMotion {
    float phase;             // 圈，[0, 1)；相差 0.5 的兩隻肢體反向擺動
    float swing;             // 度，根關節前後擺動的幅度
    float bendRest = 0.0f;   // 度，站著時彎曲關節的角度
    float bendFront = 0.0f;  // 度，肢體擺到最前方時彎曲關節多彎的角度
    float bendBack = 0.0f;   // 度，肢體擺到最後方時彎曲關節多彎的角度
};

// 依步態擺動的肢體，一段或兩段
class Limb : public Part {
   public:
    // strideLength 為公尺，走這麼遠擺一個來回
    Limb(Skeleton& skeleton, BoneIndex parent, const BoneShape& upper, const LimbMotion& motion, float strideLength);
    // lower 的 parent 一定是 upper，由這裡接上
    Limb(Skeleton& skeleton, BoneIndex parent, const BoneShape& upper, const BoneShape& lower, const LimbMotion& motion,
         float strideLength);

    BoneIndex upper() const { return _upper; }
    BoneIndex lower() const { return _lower; }  // 只有一段時為 NO_BONE

    void animate(const AnimationInput& input, Pose& pose) const override;

   private:
    BoneIndex _upper;            // 上段的骨頭編號
    BoneIndex _lower = NO_BONE;  // 下段的骨頭編號，沒有下段時為 NO_BONE
    LimbMotion _motion;
    float _strideLength;
};

#pragma endregion  // Limb

}  // namespace robot::world::entity::anatomy
