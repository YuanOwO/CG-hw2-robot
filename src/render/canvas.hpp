#pragma once

#include <string>

#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"

namespace robot::render {

// 畫布工具：在像素座標中繪製矩形、圓角矩形與文字。
//
// 座標單位為像素，原點在視窗左上角，y 向下為正。使用前須先呼叫 render::begin2D 切換到 2D 繪製。
// 所有繪製都會套用目前的座標轉換，可以用 pushTransform / translate / popTransform 平移一組繪製。
class Canvas {
   public:
    void pushTransform();  // 保存目前的座標轉換狀態
    void popTransform();   // 恢復上一個座標轉換狀態

    void translate(float x, float y);  // 平移目前座標系

    // 填滿左上角為 (x, y)、寬高為 width x height 像素的矩形。
    void fillRect(int x, int y, int width, int height, const ColorRGBA& color);

    // 填滿左上角為 (x, y)、寬高為 width x height 像素的圓角矩形；圓角以中點畫圓法逐列計算。
    void fillRoundedRect(int x, int y, int width, int height, int radius, const ColorRGBA& color);

    // 繪製圓角矩形外框：只填滿外框與內縮 thickness 像素後的內圓角矩形之間的區域，內部保持不變。
    // 內圓角半徑為 radius - thickness；radius 為 0 時即為直角外框。
    void strokeRoundedRect(int x, int y, int width, int height, int radius, int thickness, const ColorRGBA& color);

    // 以 origin 為文字框左上角繪製多行文字；換行字元 '\n' 會回到 origin.x 並下移一行。
    void drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color, const Point& origin);
};

}  // namespace robot::render
