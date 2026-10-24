#pragma once

#include "engine/input.hpp"
#include "game/camera_controller.hpp"
#include "game/camera_rig.hpp"
#include "game/player_controller.hpp"
#include "graphics/camera.hpp"
#include "math/vec3.hpp"
#include "world/entity/brain/player_brain.hpp"
#include "world/entity/entity.hpp"
#include "world/world.hpp"

namespace robot::game {

struct PlayerInfo {
    float yaw;
    float pitch;
    Vec3 velocity;
    Vec3 position;
};

class Player {
   public:
    explicit Player(world::World& world, Vec3 position);
    ~Player() = default;

    PlayerInfo info() const;
    graphics::Camera camera() const { return _cameraRig.camera(); }
    graphics::OrthoProjection projection() const { return _cameraRig.projection(); }

    void update(float dt, const engine::Input& input);
    void follow();
    void resetCamera();

   private:
    enum class CameraMode {
        FollowFace,  ///< 相機在玩家前方，玩家看向相機
        FollowBack,  ///< 相機在玩家後方，玩家看向前方
        Free,        ///< 相機不跟著玩家
    };

    float _yaw = 0.0f;
    float _pitch = 0.0f;

    CameraMode _cameraMode = CameraMode::FollowBack;

    CameraRig _cameraRig;
    CameraController _cameraController;
    PlayerController _playerController;

    world::entity::brain::PlayerBrain* _brain = nullptr;
    world::entity::Entity* _entity = nullptr;

    void syncCamera();
    void switchCameraMode();
};

}  // namespace robot::game
