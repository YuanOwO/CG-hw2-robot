#pragma once

#include "world/chunk/chunk_manager.hpp"
#include "world/collision/collider.hpp"
#include "world/physics/motion.hpp"
#include "world/physics/rigid_body.hpp"

namespace robot::world::physics {

class Simulator {
   public:
    explicit Simulator(const chunk::ChunkManager& chunkManager);
    ~Simulator() = default;

    void simulate(RigidBody& body, const collision::UprightCylinder& collider, const DriveInput& driveInput,
                  float dt) const;

   private:
    static constexpr float GROUND_SNAP_DISTANCE = 0.3f;  ///< 腳底距離地面小於此值時，會被視為站在地面上

    const chunk::ChunkManager& _chunkManager;

    Motion resolve(const Motion& origin, const collision::UprightCylinder& collider) const;

    bool isWalkable(const Vec3& normal) const;

    bool pushOutOfObstacles(collision::UprightCylinder& collider, Vec3& velocity) const;
    bool pushOutOfTerrain(collision::UprightCylinder& collider, Vec3& velocity) const;

    float supportHeightAt(const collision::UprightCylinder& collider) const;
};

}  // namespace robot::world::physics
