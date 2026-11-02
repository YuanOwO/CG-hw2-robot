#include "view/chunk_renderer.hpp"

#include <cstddef>
#include <iterator>

#include "geometry/mesh/mesh_builder.hpp"
#include "graphics/3d/primitives.hpp"
#include "math/vec3.hpp"
#include "view/collider_renderer.hpp"
#include "world/chunk/core/biome.hpp"
#include "world/chunk/core/surface.hpp"

namespace robot::view {

using world::chunk::Biome;
using world::chunk::Chunk;
using world::chunk::CHUNK_SIZE;
using world::chunk::ChunkManager;
using world::chunk::Surface;

namespace {

const ColorRGBA SURFACE_COLORS[static_cast<std::size_t>(Surface::Count)] = {
    {0.56f, 0.76f, 0.40f}, // Grass
    {0.55f, 0.53f, 0.50f}, // Stone
    {0.94f, 0.86f, 0.62f}, // Sand
    {0.95f, 0.96f, 0.98f}, // Snow
    {0.20f, 0.40f, 0.80f}, // Water
    {0.80f, 0.90f, 0.95f}, // Ice
};

ColorRGBA grassColor(Biome biome) {
    switch (biome) {
    case Biome::Plains:
        return {0.60f, 0.78f, 0.38f};  // 亮黃綠
    case Biome::Forest:
        return {0.38f, 0.62f, 0.30f};  // 中綠
    case Biome::Taiga:
        return {0.32f, 0.50f, 0.40f};  // 暗藍綠
    case Biome::Jungle:
        return {0.22f, 0.58f, 0.20f};  // 飽和深綠
    case Biome::Savanna:
        return {0.72f, 0.70f, 0.38f};  // 乾草黃
    case Biome::AlpineMeadow:
        return {0.54f, 0.74f, 0.48f};  // 淺綠
    case Biome::Swamp:
        return {0.38f, 0.45f, 0.26f};  // 暗橄欖綠
    default:
        return {0.56f, 0.76f, 0.40f};
    }
}

ColorRGBA stoneColor(Biome biome) {
    switch (biome) {
    case Biome::StonyPeaks:
        return {0.60f, 0.58f, 0.56f};
    case Biome::StoneShore:
        return {0.46f, 0.46f, 0.47f};
    case Biome::Desert:
    case Biome::Savanna:
        return {0.68f, 0.52f, 0.40f};
    case Biome::SnowyPlains:
    case Biome::SnowyTaiga:
    case Biome::SnowyPeaks:
        return {0.50f, 0.52f, 0.56f};
    default:
        return SURFACE_COLORS[static_cast<std::size_t>(Surface::Stone)];
    }
}

ColorRGBA sandColor(Biome biome) {
    switch (biome) {
    case Biome::Beach:
        return {0.94f, 0.88f, 0.68f};
    case Biome::Desert:
        return {0.90f, 0.75f, 0.50f};
    default:
        return SURFACE_COLORS[static_cast<std::size_t>(Surface::Sand)];
    }
}

ColorRGBA snowColor(Biome biome) {
    switch (biome) {
    case Biome::SnowyTaiga:
        return {0.88f, 0.92f, 0.96f};
    case Biome::SnowyPeaks:
        return {0.97f, 0.98f, 1.00f};
    default:
        return SURFACE_COLORS[static_cast<std::size_t>(Surface::Snow)];
    }
}

ColorRGBA surfaceColor(Surface surface, Biome biome) {
    switch (surface) {
    case Surface::Grass:
        return grassColor(biome);
    case Surface::Stone:
        return stoneColor(biome);
    case Surface::Sand:
        return sandColor(biome);
    case Surface::Snow:
        return snowColor(biome);
    default:
        return SURFACE_COLORS[static_cast<std::size_t>(surface)];
    }
}

ColorRGBA darker(ColorRGBA color, float factor) {
    return {color.r * factor, color.g * factor, color.b * factor, color.a};
}

}  // namespace

void ChunkRenderer::render(const ChunkManager& chunks) const {
    // 已卸載的 chunk 不會再畫到，網格留著只會一直累積
    for (auto it = _meshes.begin(); it != _meshes.end();) {
        it = chunks.chunkAt(it->first) == nullptr ? _meshes.erase(it) : std::next(it);
    }

    chunks.forEach([this](const Chunk& chunk) {
        auto it = _meshes.find(chunk.pos());
        if (it == _meshes.end()) {
            it = _meshes.emplace(chunk.pos(), buildMesh(chunk)).first;
        }
        graphics::drawMesh(it->second);

        // 畫出障礙物
        for (const auto& obstacle : chunk.obstacles()) {
            _obstacleRenderer.render(*obstacle);
        }
    });
}

geometry::Mesh ChunkRenderer::buildMesh(const Chunk& chunk) const {
    const int originX = chunk.pos().x * CHUNK_SIZE;
    const int originZ = chunk.pos().z * CHUNK_SIZE;
    auto corner = [&](int localX, int localZ) {
        return Vec3(static_cast<float>(originX + localX), chunk.vertexHeight(localX, localZ),
                    static_cast<float>(originZ + localZ));
    };

    // 每格一個四邊形，四個角依各自的高度擺放
    geometry::MeshBuilder builder;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            // 從上方看為逆時針，正面朝上
            auto a = corner(x, z);
            auto b = corner(x, z + 1);
            auto c = corner(x + 1, z + 1);
            auto d = corner(x + 1, z);
            builder.setColor(cellColor(chunk, x, z));
            builder.triangle(a, b, c).triangle(a, c, d);
        }
    }

    return builder.build();
}

ColorRGBA ChunkRenderer::cellColor(const Chunk& chunk, int localX, int localZ) const {
    const int worldX = chunk.pos().x * CHUNK_SIZE + localX;
    const int worldZ = chunk.pos().z * CHUNK_SIZE + localZ;
    const float dark = (worldX + worldZ) % 2 == 0 ? 1.0f : 0.88f;  // checkerboard pattern

    const auto surface = chunk.surfaceAt(localX, localZ);
    return darker(surfaceColor(surface, chunk.biomeAt(localX, localZ)), dark);
}

}  // namespace robot::view
