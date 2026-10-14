#include "world/entity/anatomy/upright_quadruped_rig.hpp"

#include <cmath>

namespace robot::world::entity::anatomy {

namespace {

const float EYE_HEIGHT = 0.75f;  // 頭高度的比例

}  // namespace

UprightQuadrupedRig::UprightQuadrupedRig(const QuadrupedSize& size, const QuadrupedGait& gait)
    : QuadrupedRig(size, gait) {
    const Vec3 halfBody = size.body / 2.0f;
    const Vec3 halfHead = size.head / 2.0f;

    // 頭在軀幹正上方
    addMainHead(torso(), {"head", Vec3(0.0f, size.body.y, 0.0f), Vec3(0.0f, size.head.y, 0.0f),
                          Vec3(-halfHead.x, 0.0f, -halfHead.z), Vec3(halfHead.x, size.head.y, halfHead.z)});
    addEye(Vec3(0.0f, size.head.y * EYE_HEIGHT, halfHead.z));

    // 腿貼齊軀幹左右兩側。轉軸在軀幹底面的前緣或後緣，腿整個凸出在軀幹外，
    // 只以這條邊和軀幹相接，擺動時才不會離開身體
    const Vec3 halfLeg = size.upperLeg / 2.0f;
    for (LegSlot slot : LEG_SLOTS) {
        const float side = sideOf(slot);
        const float front = frontOf(slot);
        const float farZ = front * size.upperLeg.z;
        addLeg(slot, {
                         nullptr,
                         Vec3(side * (halfBody.x - halfLeg.x), 0.0f, front * halfBody.z),
                         Vec3(0.0f, -size.upperLeg.y, farZ / 2.0f),
                         Vec3(-halfLeg.x, -size.upperLeg.y, std::fmin(0.0f, farZ)),
                         Vec3(halfLeg.x, 0.0f, std::fmax(0.0f, farZ)),
                     });
    }
}

}  // namespace robot::world::entity::anatomy
