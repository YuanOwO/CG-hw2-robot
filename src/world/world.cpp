#include "world/world.hpp"

#include "world/utils.hpp"

namespace robot::world {

World::World(Seed seed) : _seed(seed), _chunkManager(deriveSeed(seed, CHUNK_SEED_STREAM)) {
    _chunkManager.update({0, 0});  // 先載入原點
}

void World::tick(float dt) {}

}  // namespace robot::world
