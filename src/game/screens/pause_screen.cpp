#include "game/screens/pause_screen.hpp"

#include <memory>

#include "game/game.hpp"
#include "game/screens/help_screen.hpp"

namespace robot::game {

PauseScreen::PauseScreen(Game& game) : Screen(game, "已暫停") {}

void PauseScreen::build() {
    const int top = titleBottom() + 40;
    addButton(buttonSlot(0, top), "回到遊戲", [this]() { onClose(); });
    addButton(buttonSlot(1, top), "操作說明", [this]() { game().openChildScreen(std::make_unique<HelpScreen>(game())); });
    addButton(buttonSlot(2, top), "回到主選單", [this]() { game().quitToTitle(); });
}

}  // namespace robot::game
