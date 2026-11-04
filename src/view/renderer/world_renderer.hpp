#pragma once

#include "graphics/camera.hpp"
#include "view/renderer/chunk_renderer.hpp"
#include "view/renderer/entity_renderer.hpp"
#include "world/world.hpp"

namespace robot::view {

// 依照世界的狀態畫出場景，只讀取世界、不修改。
class WorldRenderer {
   public:
    void render(const world::World& world, const graphics::Camera& camera, const graphics::OrthoProjection& projection,
                int width, int height) const;

   private:
    ChunkRenderer _chunkRenderer;
    EntityRenderer _entityRenderer;
};

}  // namespace robot::view
