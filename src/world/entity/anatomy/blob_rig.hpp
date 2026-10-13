#pragma once

#include "world/entity/anatomy/rig.hpp"

namespace robot::world::entity::anatomy {

struct BlobSize {
    Vec3 body;
};

class BlobRig : public Rig {
   public:
    explicit BlobRig(const BlobSize& size);
};

}  // namespace robot::world::entity::anatomy
