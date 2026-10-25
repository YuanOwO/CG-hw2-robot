#pragma once

#include "common/color.hpp"
#include "world/chunk/chunk.hpp"

namespace robot::view {

/// 地表一格的顏色，不含依法向量的明暗；不依賴 OpenGL，離線工具也能用同一套配色
/// @param localX, localZ 格子在 chunk 內的座標，介於 0 與 CHUNK_SIZE - 1 之間
ColorRGBA terrainCellColor(const world::chunk::Chunk& chunk, int localX, int localZ);

}  // namespace robot::view
