#pragma once

#include "engine/input.hpp"
#include "world/entity/brain/brain.hpp"

namespace robot::game {

// 把按鍵與相機的視線轉成玩家指令；world 不必知道鍵盤與相機的存在
class PlayerController {
   public:
    world::entity::brain::Intent read(const engine::Input& input);
};

}  // namespace robot::game
