#pragma once

#include "graphics/canvas.hpp"
#include "view/camera_rig.hpp"

namespace robot::view {

// 遊玩時疊在場景上的資訊。
class Hud {
   public:
    void render(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond, const CameraRig& camera) const;
};

}  // namespace robot::view
