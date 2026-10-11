#pragma once

#include "screen/screen.hpp"

namespace robot::screen {

// 主選單：開始、操作說明、離開。
class TitleScreen : public Screen {
   public:
    explicit TitleScreen(ScreenHost& host);

   protected:
    void build() override;
    void renderContent(graphics::Canvas& canvas) const override;
    bool closesOnEscape() const override { return false; }
};

}  // namespace robot::screen
