#include "world/entity/anatomy/bird_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

const float WING_BEATS_PER_STRIDE = 2.0f;  // 每一步拍一下

struct WingLayout {
    const char* name;
    float side;  // +1 為左（+x）
};
const WingLayout WINGS[] = {
    {"wing_left",  1.0f },
    {"wing_right", -1.0f},
};

}  // namespace

BirdRig::BirdRig(const BirdSize& size, const BirdGait& gait) : BipedRig(size, gait) {
    const Vec3 halfBody = size.body / 2.0f;
    const Vec3 halfHead = size.head / 2.0f;
    const Vec3 halfWing = size.wing / 2.0f;

    addMainHead(torso(), {"head", Vec3(0.0f, size.body.y, 0.0f) + size.neck, Vec3(0.0f, size.head.y, 0.0f),
                          Vec3(-halfHead.x, 0.0f, -halfHead.z), Vec3(halfHead.x, size.head.y, halfHead.z)});
    addEye(Vec3(0.0f, halfHead.y, halfHead.z));  // 臉的正面，頭高度的中間

    // 翅膀貼在軀幹兩側，頂端與軀幹頂面齊平，往下垂
    for (const WingLayout& layout : WINGS) {
        const BoneShape shape = {layout.name, Vec3(layout.side * (halfBody.x + halfWing.x), size.body.y, 0.0f),
                                 Vec3(0.0f, -size.wing.y, 0.0f), Vec3(-halfWing.x, -size.wing.y, -halfWing.z),
                                 Vec3(halfWing.x, 0.0f, halfWing.z)};
        add<Wing>(torso(), shape, WingMotion{layout.side, gait.wingFlap, WING_BEATS_PER_STRIDE}, gait.strideLength);
    }
}

}  // namespace robot::world::entity::anatomy
