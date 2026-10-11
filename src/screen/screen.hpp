#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/bounding_box.hpp"
#include "common/point.hpp"
#include "gui/button.hpp"
#include "input/input_types.hpp"
#include "graphics/canvas.hpp"
#include "screen/screen_host.hpp"

namespace robot::screen {

// 疊在最上層的畫面，例如主選單、暫停選單。同一時間最多一個，由 ScreenHost 持有。
//
// 畫面可以持有上一層畫面：從暫停選單打開操作說明時，暫停選單由操作說明保管，
// 關閉操作說明就把它交還給 ScreenHost。
class Screen {
   public:
    explicit Screen(ScreenHost& host, std::string title = "");
    virtual ~Screen() = default;

    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    // 打開畫面或視窗大小改變時呼叫：清除所有按鈕後重新建立。
    void init(int width, int height);

    void render(graphics::Canvas& canvas) const;

    // 回傳是否已處理
    bool keyDown(Key key, bool repeat);
    bool mouseMove(Point position);
    bool mouseDown(MouseButton button, Point position);
    bool mouseUp(MouseButton button, Point position);

    // 預設回到上一層畫面，沒有上一層時回到遊戲。
    virtual void onClose();

    void setParent(std::unique_ptr<Screen> parent) { _parent = std::move(parent); }
    std::unique_ptr<Screen> takeParent() { return std::move(_parent); }

   protected:
    virtual void build() = 0;
    virtual void renderBackground(graphics::Canvas& canvas) const;
    virtual void renderContent(graphics::Canvas& canvas) const {}

    // 主選單沒有可以回去的地方
    virtual bool closesOnEscape() const { return true; }

    ScreenHost& host() { return _host; }
    const ScreenHost& host() const { return _host; }
    int width() const { return _width; }
    int height() const { return _height; }

    // 標題下緣的 y 座標，供子類別安排下方的內容。
    int titleBottom() const;

    void addButton(const BoundingBox& bounds, const std::string& label, gui::Button::PressHandler onPress);

    // 水平置中的按鈕欄中第 row 個按鈕的位置，第 0 個的上緣在 top。
    BoundingBox buttonSlot(int row, int top) const;

   private:
    ScreenHost& _host;
    std::string _title;
    std::unique_ptr<Screen> _parent;

    int _width = 0;
    int _height = 0;
    std::vector<gui::Button> _buttons;
    int _focusedButton = -1;  // -1：尚未用鍵盤選擇
};

}  // namespace robot::screen
