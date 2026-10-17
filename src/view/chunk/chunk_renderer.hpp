#pragma once

#include "world/chunk/chunk.hpp"

namespace robot::view::chunk {

// 畫出一個 chunk 的地面。呼叫前 MODELVIEW 須為世界座標系。
void drawChunk(const world::chunk::Chunk& chunk);

}  // namespace robot::view::chunk
