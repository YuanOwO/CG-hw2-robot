#include "view/world_renderer.hpp"

#include "graphics/primitives.hpp"
#include "graphics/stage.hpp"
#include "view/chunk/chunk_renderer.hpp"

namespace robot::view {

namespace {

const float AXIS_LENGTH = 8.0f;

}  // namespace

void WorldRenderer::render(const world::World& world, const CameraRig& camera, int width, int height) const {
    graphics::begin3D(width, height, camera.camera(), camera.projection());

    world.chunks().forEach([](const world::chunk::Chunk& c) { chunk::drawChunk(c); });

    world.entities().forEach<world::entity::Creeper>(
        [this](const world::entity::Creeper& creeper) { _creeperRenderer.render(creeper); });

    graphics::drawAxes(AXIS_LENGTH);
}

}  // namespace robot::view
