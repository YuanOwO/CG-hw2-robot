#include "screen/title_screen.hpp"

#include <string>

#include "screen/style.hpp"

namespace robot::screen {

namespace {

const std::string TITLE = "A Moving Robot";
const std::string SUBTITLE = "電腦圖學 Homework #2";

}  // namespace

TitleScreen::TitleScreen(ScreenHost& host) : Screen(host) {}

void TitleScreen::build() {
    const int top = height() / 2;
    addButton(buttonSlot(0, top), "開始", [this]() { host().startWorld(); });
    addButton(buttonSlot(1, top), "離開", [this]() { host().quit(); });
}

void TitleScreen::renderContent(graphics::Canvas& canvas) const {
    // 標題底緣在畫面中央上方 80 px，副標題接在下方 16 px。
    const int centerX = width() / 2;
    const int titleBottom = height() / 2 - 80;

    canvas.drawText(TITLE, style::TitleFont, style::Text, Point(centerX, titleBottom),
                    {graphics::TextAlign::CENTER, graphics::TextAlign::END});
    canvas.drawText(SUBTITLE, style::BodyFont, style::MutedText, Point(centerX, titleBottom + 16),
                    {graphics::TextAlign::CENTER, graphics::TextAlign::START});
}

}  // namespace robot::screen
