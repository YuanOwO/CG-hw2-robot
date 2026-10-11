#pragma once

#include <string>
#include <vector>

#include "common/bounding_box.hpp"
#include "common/point.hpp"
#include "gui/button.hpp"
#include "input/input_types.hpp"
#include "render/canvas.hpp"

namespace robot::game {

class Game;

// 疊在最上層的畫面，例如主選單。同一時間最多一個，由 Game 持有。
class Screen {
   public:
    explicit Screen(Game& game);
    virtual ~Screen() = default;

    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    // 打開畫面或視窗大小改變時呼叫：清除所有按鈕後重新建立。
    void init(int width, int height);

    void render(render::Canvas& canvas) const;

    // 回傳是否已處理
    bool mouseMove(Point position);
    bool mouseDown(MouseButton button, Point position);
    bool mouseUp(MouseButton button, Point position);

   protected:
    virtual void build() = 0;
    virtual void renderBackground(render::Canvas& canvas) const;
    virtual void renderContent(render::Canvas& canvas) const {}

    Game& game() { return _game; }
    int width() const { return _width; }
    int height() const { return _height; }

    void addButton(const BoundingBox& bounds, const std::string& label, gui::Button::PressHandler onPress);

    // 水平置中的按鈕欄中第 row 個按鈕的位置，第 0 個的上緣在 top。
    BoundingBox buttonSlot(int row, int top) const;

   private:
    Game& _game;
    int _width = 0;
    int _height = 0;
    std::vector<gui::Button> _buttons;
};

}  // namespace robot::game
