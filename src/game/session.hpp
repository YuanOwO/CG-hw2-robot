#pragma once

#include "graphics/camera.hpp"
#include "graphics/canvas.hpp"
#include "view/hud.hpp"
#include "view/world_renderer.hpp"
#include "world/world.hpp"

namespace robot::game {

// 從開始遊戲到回到主選單之間的遊玩狀態：世界，以及只在遊玩中才有意義的狀態。
// 回到主選單時整個銷毀，再次開始遊戲時因此一定從乾淨的狀態開始。
class Session {
   public:
    void tick(float dt);

    // 分成兩段，讓 Game 能在 HUD 之上接著用同一個 2D 階段畫選單。
    void renderWorld(int width, int height) const;
    void renderHud(graphics::Canvas& canvas, int framesPerSecond, int ticksPerSecond) const;

   private:
    world::World _world;

    view::WorldRenderer _worldRenderer;
    view::Hud _hud;
    graphics::Camera _camera;
    graphics::OrthoProjection _projection;
};

}  // namespace robot::game
