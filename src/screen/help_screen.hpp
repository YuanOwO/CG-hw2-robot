#pragma once

#include "screen/screen.hpp"

namespace robot::screen {

// 操作說明。從主選單或暫停選單打開，關閉後回到原本的畫面。
class HelpScreen : public Screen {
   public:
    explicit HelpScreen(ScreenHost& host);

   protected:
    void build() override;
    void renderContent(render::Canvas& canvas) const override;
};

}  // namespace robot::screen
