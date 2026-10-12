#include "world/world.hpp"

#include "math/random.hpp"

namespace robot::world {

namespace {

// 各子系統以不同的 stream 衍生 seed，改動其中一個的取亂數方式不會影響其他子系統
constexpr std::uint32_t CHUNK_SEED_STREAM = 0x43484e4bu;   // "CHNK"
constexpr std::uint32_t ENTITY_SEED_STREAM = 0x454e5459u;  // "ENTY"

std::uint64_t deriveSeed(std::uint64_t seed, std::uint32_t stream) {
    return Random(seed).valueAt(stream);
}

}  // namespace

World::World(std::uint64_t seed)
    : _seed(seed),
      _chunkManager(deriveSeed(seed, CHUNK_SEED_STREAM)),
      _entityManager(deriveSeed(seed, ENTITY_SEED_STREAM)) {}

void World::tick(float dt) {
    _entityManager.tick(dt, *this);
}

}  // namespace robot::world
