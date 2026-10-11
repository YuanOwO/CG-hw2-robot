#pragma once

#include "render/camera.hpp"
#include "world/world.hpp"

namespace robot::view {

// 依照世界的狀態畫出場景，只讀取世界、不修改。
class WorldRenderer {
   public:
    void render(const world::World& world, const render::Camera& camera, const render::OrthoProjection& projection,
                int width, int height) const;
};

}  // namespace robot::view
