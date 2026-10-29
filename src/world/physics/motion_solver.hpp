#pragma once

#include "world/chunk/chunk_manager.hpp"
#include "world/collision/collider.hpp"
#include "world/physics/motion.hpp"

namespace robot::world::physics {

class MotionSolver {
   public:
    explicit MotionSolver(const chunk::ChunkManager& chunkManager);
    ~MotionSolver() = default;

    MotionResult resolve(const collision::UprightCylinder& collider, const Motion& motion, bool wasOnGround) const;

   private:
    static constexpr float GROUND_SNAP_DISTANCE = 0.3f;  ///< 腳底距離地面小於此值時，會被視為站在地面上

    const chunk::ChunkManager& _chunkManager;

    bool isWalkable(const Vec3& normal) const;

    bool pushOutOfObstacles(collision::UprightCylinder& collider, Vec3& velocity) const;
    bool pushOutOfTerrain(collision::UprightCylinder& collider, Vec3& velocity) const;

    float supportHeightAt(const collision::UprightCylinder& collider) const;
};

}  // namespace robot::world::physics
