#pragma once

#include "engine/input.hpp"
#include "game/camera_rig.hpp"
#include "world/entity/player.hpp"

namespace robot::game {

// 把按鍵與相機的視線轉成玩家指令；world 不必知道鍵盤與相機的存在
class PlayerController {
   public:
    world::entity::PlayerCommand read(const engine::Input& input, const CameraRig& camera);

   private:
    // 解除時玩家維持原本的視線，相機才能繞到正面看玩家
    bool _followCamera = true;
};

}  // namespace robot::game
