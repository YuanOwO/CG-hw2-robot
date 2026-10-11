#pragma once

#include "graphics/canvas.hpp"

namespace robot::view {

// 遊玩時疊在場景上的資訊。
class Hud {
   public:
    void render(graphics::Canvas& canvas, int framesPerSecond, int ticksPerSecond) const;
};

}  // namespace robot::view
