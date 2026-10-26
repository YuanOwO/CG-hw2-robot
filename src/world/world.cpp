#include "world/world.hpp"

#include <memory>

#include "math/random.hpp"
#include "math/utils.hpp"
#include "world/constants.hpp"
#include "world/entity/brain/player_brain.hpp"
#include "world/entity/robot.hpp"
#include "world/utils.hpp"

namespace robot::world {

namespace {
constexpr Stream SPAWN_POSITION_SEED_STREAM = fnv1a32("SPAWN_POSITION");
constexpr float SPAWNABLE_HEIGHT = 1.0f;  // 低於這個高度的地方不會生成玩家
}  // namespace

World::World(Seed seed)
    : _seed(seed),
      _chunkManager(deriveSeed(seed, CHUNK_SEED_STREAM)),
      _entityManager(deriveSeed(seed, ENTITY_SEED_STREAM)) {}

void World::tick(float dt) {
    _entityManager.tick(dt, *this);
}

const chunk::Biome World::biomeAt(float worldX, float worldZ) const {
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
        float step = random.uniformFloat(random.valueAt1D(count), 1.0f, 5.0f) * static_cast<float>(count);
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

MotionResult World::resolveMotion(const collision::UprightCylinder& collider, const Motion& motion,
                                  bool wasOnGround) const {
    MotionResult result;
    result.motion = motion;
    result.onGround = false;

    float x = collider.baseCenter().x + result.motion.displacement.x;
    float z = collider.baseCenter().z + result.motion.displacement.z;
    float newY = collider.bottomY() + result.motion.displacement.y;
    float groundHeight = _chunkManager.heightAt(x, z);

    bool landed = newY <= groundHeight;
    bool snapped = wasOnGround && result.motion.velocity.y <= 0.0f && newY <= groundHeight + GROUND_SNAP_DISTANCE;

    // 檢查是否站在地面上
    if (landed || snapped) {
        result.motion.displacement.y = groundHeight - collider.bottomY();
        result.motion.velocity.y = 0.0f;
        result.onGround = true;
    }

    return result;
}

}  // namespace robot::world
