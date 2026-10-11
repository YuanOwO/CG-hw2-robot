#pragma once

#include "world/chunk.hpp"

namespace robot::view {

// 畫出一個 chunk 的地面與屬於它的障礙物。呼叫前 MODELVIEW 須為世界座標系。
void drawChunk(const world::Chunk& chunk);

}  // namespace robot::view
