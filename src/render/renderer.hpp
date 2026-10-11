#pragma once

#include "common/color.hpp"
#include "render/render_context.hpp"

namespace robot::render {

// 管理一幀的繪製流程。3D 場景由介面元件在繪製途中以 Scene3DScope 嵌入，
// 因此一幀只需要清除畫面、切換到 2D 介面狀態，再依介面樹的順序繪製。
class Renderer {
   public:
    // 開始新的一幀：以整個視窗為繪圖區域，清除顏色與深度緩衝區。
    void beginFrame(int width, int height, const ColorRGBA& clearColor);

    // 開始繪製 2D 介面：以整個視窗為繪圖區域，座標單位為像素、左上為原點、y 向下，關閉深度測試。
    void begin2D();

    RenderContext& canvas() { return _canvas; }

   private:
    int _width = 0;
    int _height = 0;
    RenderContext _canvas;
};

}  // namespace robot::render
