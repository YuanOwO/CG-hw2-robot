#include "screen/pause_screen.hpp"

namespace robot::screen {

PauseScreen::PauseScreen(ScreenHost& host) : Screen(host, "已暫停") {}

void PauseScreen::build() {
    const int top = titleBottom() + 40;
    // 與按 Esc 走同一條路；暫停選單沒有上一層，關閉後就回到遊戲
    addButton(buttonSlot(0, top), "回到遊戲", [this]() { onClose(); });
    addButton(buttonSlot(1, top), "回到主選單", [this]() { host().quitToTitle(); });
}

}  // namespace robot::screen
