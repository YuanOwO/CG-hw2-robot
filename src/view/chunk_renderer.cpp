#include "view/chunk_renderer.hpp"

#include <cstddef>

#include "graphics/3d/primitives.hpp"
#include "math/vec3.hpp"
#include "world/chunk/core/surface.hpp"

namespace robot::view {

using world::chunk::Chunk;
using world::chunk::CHUNK_SIZE;
using world::chunk::SurfaceType;

namespace {
const ColorRGBA SURFACE_COLORS[static_cast<std::size_t>(SurfaceType::Count)][2] = {
    {{0.56f, 0.76f, 0.40f}, {0.48f, 0.68f, 0.34f}}, // Grass
    {{0.94f, 0.86f, 0.62f}, {0.86f, 0.78f, 0.54f}}, // Sand
    {{0.55f, 0.53f, 0.50f}, {0.49f, 0.47f, 0.44f}}, // Rock
    {{0.95f, 0.96f, 0.98f}, {0.88f, 0.90f, 0.93f}}, // Snow
    {{0.20f, 0.40f, 0.80f}, {0.10f, 0.20f, 0.60f}}, // Water
    {{0.80f, 0.90f, 0.95f}, {0.70f, 0.80f, 0.90f}}, // Ice
};
}

void ChunkRenderer::render(const Chunk& chunk) const {
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
                               cellColor(chunk, x, z));
        }
    }
}

ColorRGBA ChunkRenderer::cellColor(const Chunk& chunk, int localX, int localZ) const {
    const int worldX = chunk.pos().x * CHUNK_SIZE + localX;
    const int worldZ = chunk.pos().z * CHUNK_SIZE + localZ;
    const int colorIndex = (worldX + worldZ) & 1;  // checkerboard pattern

    const auto surface = static_cast<std::size_t>(chunk.surfaceAt(localX, localZ));
    return SURFACE_COLORS[surface][colorIndex];
}

}  // namespace robot::view
