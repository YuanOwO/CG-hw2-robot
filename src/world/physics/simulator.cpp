#include "world/physics/simulator.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "math/constants.hpp"
#include "world/collision/detect.hpp"
#include "world/physics/constants.hpp"

namespace robot::world::physics {

Simulator::Simulator(const chunk::ChunkManager& chunkManager) : _chunkManager(chunkManager) {}

void Simulator::simulate(RigidBody& body, const collision::UprightCylinder& collider, const DriveInput& driveInput,
                         float dt) const {
    Vec3 targetVelocity = driveInput.targetVelocity;

    if (driveInput.jumpHeight > 0.0f) {
        float jumpSpeed = std::sqrt(2.0f * GRAVITY * driveInput.jumpHeight);
        targetVelocity.y += jumpSpeed;
    }

    // 計算速度
    Vec3 velocity = body.velocity();

    if (body.onGround()) {
        // 假設地面摩擦力足夠大
        velocity = targetVelocity;
    } else {
        velocity.y -= GRAVITY * dt;  // 空中只受重力影響
    }

    Motion motion{velocity, velocity * dt, body.onGround()};

    // 解算碰撞與地面支撐
    motion = resolve(motion, collider);

    // 更新剛體狀態
    body._position += motion.displacement;
    body._velocity = motion.velocity;
    body._onGround = motion.onGround;
}

Motion Simulator::resolve(const Motion& origin, const collision::UprightCylinder& collider) const {
    auto body = collider.translated(origin.displacement);
    auto velocity = origin.velocity;

    // 因為碰撞推擠後可能撞到新的障礙物，所以需要迭代多次
    for (int iter = 0; iter < 5; iter++) {
        bool hitObstacle = pushOutOfObstacles(body, velocity);
        bool hitTerrain = pushOutOfTerrain(body, velocity);
        if (!hitObstacle && !hitTerrain) {
            break;
        }
    }

    // 檢查是否站在地面上
    float gap = body.bottomY() - supportHeightAt(body);

    bool landed = gap <= EPSILON;
    bool snapped = origin.onGround && velocity.y <= 0.0f && gap <= GROUND_SNAP_DISTANCE;
    bool onGround = landed || snapped;

    if (onGround) {
        body = body.translated(Vec3(0.0f, -gap, 0.0f));
        velocity.y = 0.0f;
    }

    return Motion{velocity, body.baseCenter() - collider.baseCenter(), onGround};
}

bool Simulator::isWalkable(const Vec3& normal) const {
    return normal.y > SQRT_2 / 2.0f;  // 法線夾角小於 45 度，視為可行走地形
}

bool Simulator::pushOutOfObstacles(collision::UprightCylinder& collider, Vec3& velocity) const {
    bool pushed = false;

    auto collisionHandler = [&](const obstacle::Obstacle& obstacle) {
        // 檢查與障礙物的碰撞
        auto contact = collision::detect(collider, obstacle.collider());

        if (!contact.has_value()) {
            return;  // 沒有碰撞
        }

        if (contact->depth > EPSILON) {
            pushed = true;
        }

        // 依照碰撞法線調整位移與速度
        Vec3 correction = contact->normal * contact->depth;

        collider = collider.translated(-correction);

        float inwardSpeed = dot(velocity, contact->normal);
        if (inwardSpeed > 0.0f) {
            velocity -= contact->normal * inwardSpeed;
        }
    };

    _chunkManager.forEachObstacleNear(collider.baseCenter().x, collider.baseCenter().z, collisionHandler);

    return pushed;
}

bool Simulator::pushOutOfTerrain(collision::UprightCylinder& collider, Vec3& velocity) const {
    bool pushed = false;

    float groundHeight = _chunkManager.heightAt(collider.baseCenter().x, collider.baseCenter().z);
    Vec3 groundNormal = _chunkManager.normalAt(collider.baseCenter().x, collider.baseCenter().z);

    if (collider.baseCenter().y < groundHeight && !isWalkable(groundNormal)) {
        pushed = true;

        float cotangent = groundNormal.y / std::hypot(groundNormal.x, groundNormal.z);

        // 地形陡峭，視為撞到牆壁，不能踩上去
        collision::Contact contact;
        contact.normal = -normalize(Vec3(groundNormal.x, 0.0f, groundNormal.z));  // 法線只取水平分量
        contact.depth = (groundHeight - collider.baseCenter().y) * cotangent;

        // 依照碰撞法線調整位移與速度
        Vec3 correction = contact.normal * contact.depth;

        collider = collider.translated(-correction);

        float inwardSpeed = dot(velocity, contact.normal);
        if (inwardSpeed > 0.0f) {
            velocity -= contact.normal * inwardSpeed;
        }
    }

    return pushed;
}

float Simulator::supportHeightAt(const collision::UprightCylinder& collider) const {
    float supportHeight = std::numeric_limits<float>::lowest();

    auto probe = collider.translated(Vec3(0.0f, -1e-3f, 0.0f));  // 往下探一點，避免浮點誤差導致碰撞偵測失敗

    auto collisionHandler = [&](const obstacle::Obstacle& obstacle) {
        auto contact = collision::detect(probe, obstacle.collider());

        if (!contact.has_value()) {
            return;  // 沒有碰撞
        }

        if (!isWalkable(-contact->normal)) {
            return;  // 側面或太陡的表面不能站
        }

        supportHeight = std::max(supportHeight, collider.bottomY());
    };

    float x = collider.baseCenter().x;
    float z = collider.baseCenter().z;

    _chunkManager.forEachObstacleNear(x, z, collisionHandler);

    float terrainHeight = _chunkManager.heightAt(x, z);
    Vec3 terrainNormal = _chunkManager.normalAt(x, z);

    if (isWalkable(terrainNormal)) {
        supportHeight = std::max(supportHeight, terrainHeight);
    }

    return supportHeight;
}

}  // namespace robot::world::physics
