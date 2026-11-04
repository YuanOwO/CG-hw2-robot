#pragma once

#include <unordered_map>

#include "common/color.hpp"
#include "geometry/mesh/mesh.hpp"
#include "view/renderer/obstacle_renderer.hpp"
#include "world/chunk/chunk.hpp"
#include "world/chunk/chunk_manager.hpp"

namespace robot::view {

/// 把地形畫成每格一個四邊形。每個 chunk 組成一個網格，第一次畫到時建立，之後重複使用
class ChunkRenderer {
   public:
    /// 畫出所有已載入的 chunk，並丟掉已卸載 chunk 的網格
    void render(const world::chunk::ChunkManager& chunks) const;

   private:
    ObstacleRenderer _obstacleRenderer;

    // 同一個位置重新載入時生成的地形相同，所以網格以位置為 key，卸載前不必重建。
    // 快取不影響畫出的結果，因此在 const 的 render 中也可以更新
    mutable std::unordered_map<world::chunk::ChunkPos, geometry::Mesh, world::chunk::ChunkPosHash> _meshes;

    geometry::Mesh buildMesh(const world::chunk::Chunk& chunk) const;
    ColorRGBA cellColor(const world::chunk::Chunk& chunk, int localX, int localZ) const;
};

}  // namespace robot::view
