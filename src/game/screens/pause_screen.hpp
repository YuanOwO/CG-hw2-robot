#pragma once

#include "game/screen.hpp"

namespace robot::game {

// 暫停選單：遊玩中按 Esc 打開，世界停止更新但仍畫在後面。
class PauseScreen : public Screen {
   public:
    explicit PauseScreen(Game& game);

   protected:
    void build() override;
};

}  // namespace robot::game
