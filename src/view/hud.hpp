#pragma once

#include "graphics/2d/canvas.hpp"
#include "math/vec3.hpp"
#include "world/chunk/core/biome.hpp"

namespace robot::view {

// HUD 要顯示的數值，由呼叫的一方填好
struct HudInfo {
    double framesPerSecond;
    double ticksPerSecond;
    float cameraYaw;
    float cameraPitch;
    Vec3 playerVelocity;
    Vec3 playerPosition;
    world::chunk::Biome playerBiome;
};

// 遊玩時疊在場景上的資訊。
class Hud {
   public:
    void render(graphics::Canvas& canvas, const HudInfo& info) const;
};

}  // namespace robot::view
