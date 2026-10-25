#include "view/chunk_renderer.hpp"

#include "graphics/3d/primitives.hpp"
#include "math/vec3.hpp"
#include "view/terrain_palette.hpp"

namespace robot::view {

void ChunkRenderer::render(const world::chunk::Chunk& chunk) const {
    using world::chunk::CHUNK_SIZE;

    const int originX = chunk.pos().x * CHUNK_SIZE;
    const int originZ = chunk.pos().z * CHUNK_SIZE;
    auto corner = [&](int localX, int localZ) {
        return Vec3(static_cast<float>(originX + localX), chunk.vertexHeight(localX, localZ),
                    static_cast<float>(originZ + localZ));
    };

    // 每格一個四邊形，四個角依各自的高度擺放
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            // 從上方看為逆時針，正面朝上
            graphics::drawQuad(corner(x, z), corner(x, z + 1), corner(x + 1, z + 1), corner(x + 1, z),
                               terrainCellColor(chunk, x, z));
        }
    }
}

}  // namespace robot::view
