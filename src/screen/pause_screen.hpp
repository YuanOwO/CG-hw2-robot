#pragma once

#include "screen/screen.hpp"

namespace robot::screen {

// 暫停選單：遊玩中按 Esc 打開，世界停止更新但仍畫在後面。
class PauseScreen : public Screen {
   public:
    explicit PauseScreen(ScreenHost& host);

   protected:
    void build() override;
};

}  // namespace robot::screen
