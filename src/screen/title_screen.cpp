#include "screen/title_screen.hpp"

#include <memory>
#include <string>

#include "screen/help_screen.hpp"
#include "screen/style.hpp"

namespace robot::screen {

namespace {

const std::string TITLE = "一個移動的羅伯特";
const std::string SUBTITLE = "電腦圖學 Homework #2";

}  // namespace

TitleScreen::TitleScreen(ScreenHost& host) : Screen(host) {}

void TitleScreen::build() {
    const int top = height() / 2;
    addButton(buttonSlot(0, top), "開始", [this]() { host().startWorld(); });
    addButton(buttonSlot(1, top), "操作說明",
              [this]() { host().openChildScreen(std::make_unique<HelpScreen>(host())); });
    addButton(buttonSlot(2, top), "離開", [this]() { host().quit(); });
}

void TitleScreen::renderContent(render::Canvas& canvas) const {
    // 標題底緣在畫面中央上方 80 px，副標題接在下方 16 px。
    const float centerX = width() / 2.0f;
    const float titleBottom = height() / 2.0f - 80.0f;

    canvas.drawText(TITLE, style::TitleFont, style::Text, Point(centerX, titleBottom),
                    {render::TextAlign::CENTER, render::TextAlign::END});
    canvas.drawText(SUBTITLE, style::BodyFont, style::MutedText, Point(centerX, titleBottom + 16.0f),
                    {render::TextAlign::CENTER, render::TextAlign::START});
}

}  // namespace robot::screen
