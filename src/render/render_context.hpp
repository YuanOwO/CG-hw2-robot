#pragma once

#include <string>

#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"

namespace robot::render {

class RenderContext {
   public:
    ~RenderContext() = default;

    // 繪圖表面（視窗）的像素尺寸；由 Renderer 在每幀設定，供需要換算 OpenGL 視窗座標的繪製使用。
    int surfaceWidth() const { return _surfaceWidth; }
    int surfaceHeight() const { return _surfaceHeight; }
    void setSurfaceSize(int width, int height) {
        _surfaceWidth = width;
        _surfaceHeight = height;
    }

    void pushTransform();  // 保存目前的座標轉換狀態
    void popTransform();   // 恢復上一個座標轉換狀態

    void translate(float x, float y);  // 平移目前座標系
    void fillRect(int width, int height, const ColorRGBA& color);

    // 以中點畫圓法繪製 1px 寬的圓周；圓心與半徑皆為整數像素。
    void strokeCircle(int centerX, int centerY, int radius, const ColorRGBA& color);

    // 填滿內外兩個中點圓之間的環形區域，包含兩個圓周本身；逐列填滿，不會留下空洞。
    void fillRing(int centerX, int centerY, int innerRadius, int outerRadius, const ColorRGBA& color);

    // 填滿左上角為 (x, y)、寬高為 width x height 像素的圓角矩形；圓角以中點畫圓法逐列計算。
    void fillRoundedRect(int x, int y, int width, int height, int radius, const ColorRGBA& color);

    // 繪製圓角矩形外框：只填滿外框與內縮 thickness 像素後的內圓角矩形之間的區域，內部保持不變。
    // 內圓角半徑為 radius - thickness；radius 為 0 時即為直角外框。
    void strokeRoundedRect(int x, int y, int width, int height, int radius, int thickness, const ColorRGBA& color);

    // 以 origin 為文字框左上角繪製多行文字；換行字元 '\n' 會回到 origin.x 並下移一行。
    void drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color, const Point& origin);

   private:
    int _surfaceWidth = 0;
    int _surfaceHeight = 0;
};

}  // namespace robot::render
