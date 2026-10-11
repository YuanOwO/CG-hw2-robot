#pragma once

#include "engine/input.hpp"
#include "render/camera.hpp"

namespace robot::game {

// 遊戲世界：場景中的物體與它們的狀態。目前是一塊繞中心旋轉的地板。
class World {
   public:
    void tick(float dt, const engine::Input& input);
    void render(int width, int height) const;

   private:
    float _angle = 0.0f;  // 地板繞 y 軸旋轉的角度（度）

    render::Camera _camera;
    render::OrthoProjection _projection;
};

}  // namespace robot::game
