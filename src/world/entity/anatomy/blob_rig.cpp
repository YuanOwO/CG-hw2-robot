#include "world/entity/anatomy/blob_rig.hpp"

namespace robot::world::entity::anatomy {

namespace {

const float EYE_HEIGHT = 0.75f;  // 身體高度的比例

}  // namespace

BlobRig::BlobRig(const BlobSize& size) {
    const Vec3 half = size.body / 2.0f;

    // 轉軸在底面中心，身體貼著地面
    addMainHead(ROOT_BONE,
                {"body", Vec3(), Vec3(0.0f, size.body.y, 0.0f), Vec3(-half.x, 0.0f, -half.z),
                 Vec3(half.x, size.body.y, half.z)},
                JointLimits{});

    // 臉的正面
    addEye(Vec3(0.0f, size.body.y * EYE_HEIGHT, half.z));
}

}  // namespace robot::world::entity::anatomy
