#pragma once

#include "engine/application.hpp"

namespace robot::game {

// 遊戲本身。目前只有一塊繞中心旋轉的地板，用來確認主迴圈。
class Game : public engine::Application {
   public:
    void tick(float dt) override;
    void render() override;

   private:
    float _angle = 0.0f;  // 地板繞 y 軸旋轉的角度（度）
};

}  // namespace robot::game
