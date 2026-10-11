#include "app/windows/world.hpp"

#include <memory>
#include <utility>

#include "common/color.hpp"
#include "event/input_event.hpp"
#include "input/input_types.hpp"
#include "ui/elements.hpp"
#include "ui/theme.hpp"

namespace robot::app {

WorldWindow::WorldWindow(const std::string& title, int width, int height) : Window(title, width, height) {
    setupContent();
    setupMenu();
    setupShortcuts();

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (_shortcutManager.handle(event)) {
            event.stopPropagation();
        }
    });
}

#pragma region Content Setup

void WorldWindow::setupContent() {
    auto dock = std::make_unique<ui::DockPanelElement>();

    auto statusBar = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);
    statusBar->setBackgroundColor(Color::LightGray);
    statusBar->setPadding({8, 4, 8, 4});

    // 設置狀態列的文字元素

    auto statusText = std::make_unique<ui::TextElement>("Ready", ui::theme::BodyFont);
    _statusText = statusText.get();
    statusBar->appendChild(std::move(statusText), 1.0f);

    // 設置 DockPanel 的內容

    dock->appendChild(std::move(statusBar), ui::Dock::Bottom);

    // 設置內容
    setContent(std::move(dock));
}

#pragma endregion  // Content Setup

#pragma region Shortcuts

void WorldWindow::setupShortcuts() {
    _shortcutManager.bind({Mod::Ctrl, Key::C}, [this]() { close(); });
}

#pragma endregion  // Shortcuts

#pragma region Main Menu

void WorldWindow::setupMenu() {
    _menu.addMenuEntry("Quit", [this]() { close(); });

    _menu.attach(MouseButton::MouseRight);
}

#pragma endregion  // Main Menu

}  // namespace robot::app
