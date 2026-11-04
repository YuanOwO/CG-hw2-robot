#include "world/world.hpp"

#include <cmath>

#include "math/random.hpp"
#include "math/utils.hpp"
#include "world/utils.hpp"

namespace robot::world {

namespace {
constexpr Stream SPAWN_POSITION_SEED_STREAM = fnv1a32("SPAWN_POSITION");
constexpr float SPAWNABLE_HEIGHT = 1.0f;  // 低於這個高度的地方不會生成玩家
}  // namespace

World::World(Seed seed)
    : _seed(seed),
      _chunkManager(deriveSeed(seed, CHUNK_SEED_STREAM)),
      _entityManager(deriveSeed(seed, ENTITY_SEED_STREAM)),
      _simulator(_chunkManager) {}

void World::tick(float dt) {
    _entityManager.tick(dt, *this);
}

chunk::Biome World::biomeAt(float worldX, float worldZ) const {
    return _chunkManager.biomeAt(worldX, worldZ);
}

Vec3 World::spawnPosition() const {
    Random random(deriveSeed(_seed, SPAWN_POSITION_SEED_STREAM));
    int count = 0;
    float x = 0.0f, z = 0.0f;

    while (count < 1000) {  // 最多嘗試 1000 次
        const float y = _chunkManager.heightAt(x, z);
        if (y >= SPAWNABLE_HEIGHT) {
            return {x, y, z};
        }

        int direction = ++count % 4;
        float step = static_cast<float>(Random::uniformFloat(random.valueAt1D(count), 1.0f, 5.0f));
        step *= static_cast<float>(count);

        if (direction == 0) {
            z += step;
        } else if (direction == 1) {
            x += step;
        } else if (direction == 2) {
            z -= step;
        } else {
            x -= step;
        }
        x = std::round(x);
        z = std::round(z);
    }

    return {0.0f, _chunkManager.heightAt(0.0f, 0.0f), 0.0f};  // 找不到合適位置就回傳原點
}

void World::setFocus(Vec3 position) {
    _chunkManager.update(chunk::ChunkPos::containing(position.x, position.z));
}

}  // namespace robot::world
