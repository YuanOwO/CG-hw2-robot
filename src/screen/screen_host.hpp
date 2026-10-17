#pragma once

#include <memory>

namespace robot::screen {

class Screen;

// 畫面所需的遊戲操作。由持有畫面的一方實作，畫面因此不必認識遊戲本身。
class ScreenHost {
   public:
    virtual ~ScreenHost() = default;

    // 畫面依此決定背景：疊在世界上，或是獨立的選單
    virtual bool hasWorld() const = 0;

    virtual void setScreen(std::unique_ptr<Screen> screen) = 0;  // nullptr 表示關閉畫面

    // 目前的畫面成為新畫面的上一層，關閉新畫面時回到它。
    virtual void openChildScreen(std::unique_ptr<Screen> screen) = 0;

    virtual void startWorld() = 0;   // 建立新世界並關閉畫面
    virtual void quitToTitle() = 0;  // 銷毀世界並回到主選單
    virtual void quit() = 0;
};

}  // namespace robot::screen
