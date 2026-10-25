#include "view/terrain_palette.hpp"

namespace robot::view {

namespace {

const ColorRGBA GRASS[2] = {
    {0.56f, 0.76f, 0.40f},
    {0.48f, 0.68f, 0.34f}
};

const ColorRGBA WATER[2] = {
    {0.2f, 0.4f, 0.8f},
    {0.1f, 0.2f, 0.6f}
};

const ColorRGBA SAND[2] = {
    {0.94f, 0.86f, 0.62f},
    {0.86f, 0.78f, 0.54f}
};

}  // namespace

ColorRGBA terrainCellColor(const world::chunk::Chunk& chunk, int localX, int localZ) {
    using world::chunk::CHUNK_SIZE;

    const int worldX = chunk.pos().x * CHUNK_SIZE + localX;
    const int worldZ = chunk.pos().z * CHUNK_SIZE + localZ;

    // 兩色交錯，移動時才看得出走了多遠；以世界座標決定顏色，相鄰 chunk 的格子才接得上
    // 用 & 1 而非 % 2：負座標的 % 2 是 -1，會讀到陣列外
    const int colorIndex = (worldX + worldZ) & 1;

    // 取四個角的平均，而不是只看一角，否則分界會整體偏移半格
    const float height = (chunk.vertexHeight(localX, localZ) + chunk.vertexHeight(localX + 1, localZ) +
                          chunk.vertexHeight(localX, localZ + 1) + chunk.vertexHeight(localX + 1, localZ + 1)) /
                         4.0f;

    if (height < 0.0f) {
        return WATER[colorIndex];
    }
    if (height < 2.0f) {
        return SAND[colorIndex];
    }
    return GRASS[colorIndex];
}

}  // namespace robot::view
