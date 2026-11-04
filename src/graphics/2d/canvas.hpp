#pragma once

#include <string>

#include "common/color.hpp"
#include "common/point.hpp"
#include "font/font.hpp"

namespace robot::graphics {

/// 文字框沿某個方向的對齊方式
enum class TextAlign {
    START,   ///< 左 / 上
    CENTER,  ///< 中央
    END,     ///< 右 / 下
};

/// 指定的位置是文字框的哪一點，例如 {CENTER, CENTER} 為文字框中心
struct TextAnchor {
    TextAlign horizontal = TextAlign::START;
    TextAlign vertical = TextAlign::START;
};

/// 在像素座標中繪製矩形、圓角矩形與文字。
/// 座標單位為像素，原點在視窗左上角、y 向下，使用前須先呼叫 begin2D。
/// 所有繪製都套用目前的座標轉換，可以用 pushTransform、translate、popTransform 平移一組繪製
class Canvas {
   public:
    ~Canvas() = default;

    /// 保存目前的座標轉換，與 popTransform 成對使用
    void pushTransform();
    /// 恢復到上一次 pushTransform 時的座標轉換
    void popTransform();
    /// 平移之後的繪製，單位為像素
    void translate(int x, int y);

    /// 填滿矩形；寬或高不大於 0、或完全透明時不畫
    /// @param x, y 左上角
    /// @param width, height 寬高，像素
    void fillRect(int x, int y, int width, int height, const ColorRGBA& color);

    /// 畫矩形的外框，內部不畫；寬、高或粗細不大於 0、或完全透明時不畫
    /// @param x, y 左上角
    /// @param width, height 外框的寬高，像素
    /// @param thickness 框線粗細，像素
    void strokeRect(int x, int y, int width, int height, int thickness, const ColorRGBA& color);

    /// 畫多行文字，以 '\n' 換行，每行靠文字框左緣。
    /// 位置與文字框大小都是整數像素，字形才不會落在像素之間而模糊或抖動
    /// @param position 文字框上由 anchor 指定的那一點
    /// @param anchor 預設 position 為文字框左上角
    void drawText(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color, const Point& position,
                  TextAnchor anchor = {});
};

}  // namespace robot::graphics
