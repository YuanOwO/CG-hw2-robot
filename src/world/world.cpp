#include "world/world.hpp"

#include <memory>

#include "world/constants.hpp"
#include "world/entity/brain/player_brain.hpp"
#include "world/entity/robot.hpp"
#include "world/utils.hpp"

namespace robot::world {

World::World(Seed seed)
    : _seed(seed),
      _chunkManager(deriveSeed(seed, CHUNK_SEED_STREAM)),
      _entityManager(deriveSeed(seed, ENTITY_SEED_STREAM)) {
    setFocus({0.0f, 0.0f, 0.0f});  // 先載入原點
}

void World::tick(float dt) {
    _entityManager.tick(dt, *this);
}

void World::setFocus(Vec3 position) {
    _chunkManager.update(chunk::ChunkPos::containing(position.x, position.z));
}

std::optional<float> World::heightAt(float worldX, float worldZ) const {
    const chunk::Chunk* chunk = _chunkManager.chunkAt(chunk::ChunkPos::containing(worldX, worldZ));
    if (chunk == nullptr) {
        return std::nullopt;
    }
    return chunk->heightAt(worldX, worldZ);
}

MotionResult World::resolveMotion(const collision::UprightCylinder& collider, const Motion& motion,
                                  bool wasOnGround) const {
    MotionResult result;
    result.motion = motion;
    result.onGround = false;

    float x = collider.baseCenter().x + result.motion.displacement.x;
    float z = collider.baseCenter().z + result.motion.displacement.z;
    float newY = collider.bottomY() + result.motion.displacement.y;
    float groundHeight = heightAt(x, z).value_or(-INFINITY);

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
