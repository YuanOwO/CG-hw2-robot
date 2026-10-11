#pragma once

#include "game/screen.hpp"

namespace robot::game {

// 主選單：開始、離開。
class TitleScreen : public Screen {
   public:
    explicit TitleScreen(Game& game);

   protected:
    void build() override;
    void renderContent(render::Canvas& canvas) const override;
};

}  // namespace robot::game
