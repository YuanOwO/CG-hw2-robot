#include "view/world_renderer.hpp"

#include "graphics/3d/primitives.hpp"
#include "graphics/render_mode.hpp"
#include "view/chunk/chunk_renderer.hpp"

namespace robot::view {

namespace {

const float AXIS_LENGTH = 8.0f;

}  // namespace

void WorldRenderer::render(const world::World& world, const graphics::Camera& camera,
                           const graphics::OrthoProjection& projection, int width, int height) const {
    graphics::begin3D(width, height, camera, projection);

    world.chunks().forEach([](const world::chunk::Chunk& c) { chunk::drawChunk(c); });

    // world.entities().forEach<world::entity::LivingEntity>(
    //     [this](const world::entity::LivingEntity& entity) { _skeletonRenderer.render(entity); });

    graphics::drawAxes(AXIS_LENGTH);
}

}  // namespace robot::view
