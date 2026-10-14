#include "world/entity/anatomy/horizontal_quadruped_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

const float EYE_HEIGHT = 0.75f;  // 頭高度的比例

}  // namespace

HorizontalQuadrupedRig::HorizontalQuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait)
    : QuadrupedRig(size, gait) {
    const Vec3 halfBody = size.body / 2.0f;
    const Vec3 halfHead = size.head / 2.0f;

    // 脖子在軀幹正面的頂端，頭從這裡往前長、上下各占一半，抬頭低頭時才不會穿進軀幹
    addMainHead(torso(), {"head", Vec3(0.0f, size.body.y, halfBody.z), Vec3(0.0f, halfHead.y, 0.0f),
                          Vec3(-halfHead.x, -halfHead.y, 0.0f), Vec3(halfHead.x, halfHead.y, size.head.z)});
    addEye(Vec3(0.0f, size.head.y * (EYE_HEIGHT - 0.5f), size.head.z));

    // 腿在軀幹底面的四個角，外側與軀幹齊平，垂直往下
    const Vec3 halfLeg = size.upperLeg / 2.0f;
    for (LegSlot slot : LEG_SLOTS) {
        const float side = sideOf(slot);
        const float front = frontOf(slot);
        addLeg(slot, {
                         nullptr,
                         Vec3(side * (halfBody.x - halfLeg.x), 0.0f, front * (halfBody.z - halfLeg.z)),
                         Vec3(0.0f, -size.upperLeg.y, 0.0f),
                         Vec3(-halfLeg.x, -size.upperLeg.y, -halfLeg.z),
                         Vec3(halfLeg.x, 0.0f, halfLeg.z),
                     });
    }
}

}  // namespace robot::world::entity::anatomy
