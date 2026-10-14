#pragma once

#include <cstdint>

#include "engine/input.hpp"
#include "game/camera_controller.hpp"
#include "game/player_controller.hpp"
#include "graphics/canvas.hpp"
#include "view/camera_rig.hpp"
#include "view/hud.hpp"
#include "view/world_renderer.hpp"
#include "world/world.hpp"

namespace robot::game {

// 從開始遊戲到回到主選單之間的遊玩狀態：世界，以及只在遊玩中才有意義的狀態。
// 回到主選單時整個銷毀，再次開始遊戲時因此一定從乾淨的狀態開始。
class Session {
   public:
    Session();

    void tick(float dt, const engine::Input& input);
    // 有畫面蓋住、停止 tick 時呼叫，清掉停止期間會失效的操作狀態
    void pause();

    // 分成兩段，讓 Game 能在 HUD 之上接著用同一個 2D 階段畫選單。
    void renderWorld(int width, int height) const;
    void renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond) const;

   private:
    std::uint64_t _seed = 24622192;  // 隨便挑的數字，讓每次遊戲都一樣

    world::World _world;
    view::CameraRig _cameraRig;
    CameraController _cameraController;
    PlayerController _playerController;

    view::WorldRenderer _worldRenderer;
    view::Hud _hud;
};

}  // namespace robot::game
