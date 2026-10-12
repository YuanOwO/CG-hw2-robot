#pragma once

#include "view/camera_rig.hpp"
#include "view/entity/creeper_renderer.hpp"
#include "world/world.hpp"

namespace robot::view {

// 依照世界的狀態畫出場景，只讀取世界、不修改。
class WorldRenderer {
   public:
    void render(const world::World& world, const CameraRig& camera, int width, int height) const;

   private:
    entity::CreeperRenderer _creeperRenderer;
};

}  // namespace robot::view
