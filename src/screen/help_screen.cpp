#include "screen/help_screen.hpp"

#include <string>
#include <utility>
#include <vector>

#include "screen/style.hpp"

namespace robot::screen {

namespace {

const int LINE_HEIGHT = 28;

const std::vector<std::pair<std::string, std::string>> CONTROLS = {
    {"左鍵拖曳", "轉動視角"   },
    {"Q E",      "環繞視角"   },
    {"Esc",      "暫停 / 返回"},
    {"↑ ↓",      "選擇按鈕"   },
    {"Enter",    "確認"       },
};

// build 與 renderContent 共用，按鈕才會接在說明列表下方
int contentTop(int titleBottom) {
    return titleBottom + 32;
}

}  // namespace

HelpScreen::HelpScreen(ScreenHost& host) : Screen(host, "操作說明") {}

void HelpScreen::build() {
    const int top = contentTop(titleBottom()) + static_cast<int>(CONTROLS.size()) * LINE_HEIGHT + 32;
    addButton(buttonSlot(0, top), "返回", [this]() { onClose(); });
}

void HelpScreen::renderContent(graphics::Canvas& canvas) const {
    // 按鍵靠右對齊在中線左側，說明從中線右側開始，兩欄才會對齊
    const float center = width() / 2.0f;
    float y = static_cast<float>(contentTop(titleBottom()));

    for (const auto& [key, description] : CONTROLS) {
        canvas.drawText(key, style::BodyFont, style::MutedText, Point(center - 16.0f, y),
                        {graphics::TextAlign::END, graphics::TextAlign::START});
        canvas.drawText(description, style::BodyFont, style::Text, Point(center + 16.0f, y));
        y += LINE_HEIGHT;
    }
}

}  // namespace robot::screen
