#include "view/world_renderer.hpp"

#include "graphics/primitives.hpp"
#include "graphics/stage.hpp"
#include "view/chunk_renderer.hpp"

namespace robot::view {

namespace {

const float AXIS_LENGTH = 8.0f;

}  // namespace

void WorldRenderer::render(const world::World& world, const graphics::Camera& camera,
                           const graphics::OrthoProjection& projection, int width, int height) const {
    graphics::begin3D(width, height, camera, projection);

    world.forEachChunk([](const world::Chunk& chunk) { drawChunk(chunk); });

    graphics::drawAxes(AXIS_LENGTH);
}

}  // namespace robot::view
