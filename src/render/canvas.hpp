#pragma once

#include <string>

#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"

namespace robot::render {

enum class TextAlign {
    START,   // 左 / 上
    CENTER,  // 中央
    END,     // 右 / 下
};

struct TextAnchor {
    TextAlign horizontal = TextAlign::START;
    TextAlign vertical = TextAlign::START;
};

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

    // 繪製多行文字，以 '\n' 換行，每行都靠文字框左緣。
    // anchor 決定 position 是文字框的哪一點，例如 {CENTER, CENTER} 時 position 為文字框中心；
    // 文字框的左上角會取整到整數像素，字形才不會因為落在像素之間而模糊或抖動。
    void drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color, const Point& position,
                  TextAnchor anchor = {});
};

}  // namespace robot::render
