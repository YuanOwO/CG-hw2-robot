#pragma once

#include "engine/application.hpp"
#include "engine/engine.hpp"
#include "render/camera.hpp"

namespace robot::game {

// 遊戲本身。目前是一塊繞中心旋轉的地板，加上左上角的 FPS / TPS。
class Game : public engine::Application {
   public:
    explicit Game(engine::Engine& engine);

    void tick(float dt) override;
    void render() override;

   private:
    engine::Engine& _engine;
    float _angle = 0.0f;  // 地板繞 y 軸旋轉的角度（度）

    // 從斜上方看向地板中心；投影使用作業題目的範圍
    render::Camera _camera{{55.0f, 70.0f, 55.0f}, {25.0f, 0.0f, 25.0f}, {0.0f, 1.0f, 0.0f}};
    render::OrthoProjection _projection;

    void renderWorld();  // 3D 場景
    void renderHud();    // 疊在場景上的 2D 介面
};

}  // namespace robot::game
