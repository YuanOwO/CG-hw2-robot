#pragma once

#include <string>

#include "input/shortcut_manager.hpp"
#include "ui/elements/content/text.hpp"
#include "ui/menu.hpp"
#include "ui/window.hpp"

namespace robot::app {

class WorldWindow : public ui::Window {
   public:
    WorldWindow(const std::string& title, int width, int height);

   private:
    ShortcutManager _shortcutManager;
    ui::Menu _menu;

    ui::TextElement* _statusText = nullptr;

    void setupContent();
    void setupShortcuts();
    void setupMenu();
};

}  // namespace robot::app
