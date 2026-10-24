#include "view/chunk/chunk_renderer.hpp"

#include "common/color.hpp"
#include "graphics/3d/primitives.hpp"
#include "math/vec3.hpp"

namespace robot::view::chunk {

namespace {

const ColorRGBA GRASS_LIGHT{0.56f, 0.76f, 0.40f};
const ColorRGBA GRASS_DARK{0.48f, 0.68f, 0.34f};

}  // namespace

void drawChunk(const world::chunk::Chunk& chunk) {
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
            // 兩色交錯，移動時才看得出走了多遠；以世界座標決定顏色，相鄰 chunk 的格子才接得上
            const ColorRGBA& color = (originX + x + originZ + z) % 2 == 0 ? GRASS_LIGHT : GRASS_DARK;
            // 從上方看為逆時針，正面朝上
            graphics::drawQuad(corner(x, z), corner(x, z + 1), corner(x + 1, z + 1), corner(x + 1, z), color);
        }
    }
}

}  // namespace robot::view::chunk
