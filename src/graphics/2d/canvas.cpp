#include "graphics/2d/canvas.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "platform/glut.hpp"

namespace robot::graphics {
namespace {

// 中點畫圓法：依序回傳第二個八分圓 (0 <= x <= y) 上的整數點，其餘七個八分圓由呼叫端以對稱取得。
template <typename Visit>
void midpointOctant(int radius, Visit visit) {
    int x = 0;
    int y = radius;
    int decision = 1 - radius;
    while (x <= y) {
        visit(x, y);

        // decision 為中點 (x + 1, y - 1/2) 的判別式：小於 0 表示中點在圓內，y 保持不變。
        if (decision < 0) {
            decision += 2 * x + 3;
        } else {
            decision += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// 回傳中點圓在每一列 (以 |dy| 為索引) 最外側的 |x|。
// 八分圓的點 (x, y) 同時落在第 y 列 (|x| = x) 與第 x 列 (|x| = y)。
std::vector<int> rowExtents(int radius) {
    std::vector<int> extents(radius + 1, -1);
    midpointOctant(radius, [&extents](int x, int y) {
        extents[y] = std::max(extents[y], x);
        extents[x] = std::max(extents[x], y);
    });
    return extents;
}

// 開始以 GL_QUADS 逐像素繪製；呼叫端須先 glPushAttrib，結束後 glEnd 與 glPopAttrib。
void beginPixels(const ColorRGBA& color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);
}

// 在 glBegin(GL_QUADS) 區段內畫出從 (x, y) 開始、寬 width 像素、高 1 像素的水平區段。
// 以方塊而非 GL_POINTS 繪製，才會跟隨目前座標系縮放。
void pixelRect(int x, int y, int width) {
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + 1);
    glVertex2i(x, y + 1);
}

// 圓角矩形在第 row 列 (0 <= row < height) 的水平範圍，以相對於矩形左緣的 [left, right] 表示。
// radius 須已限制在 (min(width, height) - 1) / 2 以內，extents 為 rowExtents(radius)。
std::pair<int, int> roundedRowSpan(int width, int height, int radius, const std::vector<int>& extents, int row) {
    // dy 為此列到最近圓心列的距離；位於上下圓心之間的列 dy = 0，整列填滿。
    int dy = 0;
    if (row < radius) {
        dy = radius - row;
    } else if (row > height - 1 - radius) {
        dy = row - (height - 1 - radius);
    }

    // 左右圓角的圓心欄位；中間的直邊由兩者之間的水平區段自然連接。
    return {radius - extents[dy], width - 1 - radius + extents[dy]};
}

// 圓心落在像素中心，直徑為 2r + 1 像素；限制半徑讓上下、左右兩側的圓角不會交疊。
int clampCornerRadius(int width, int height, int radius) {
    return std::clamp(radius, 0, (std::min(width, height) - 1) / 2);
}

}  // namespace

void Canvas::pushTransform() {
    glPushMatrix();
}

void Canvas::popTransform() {
    glPopMatrix();
}

void Canvas::translate(float x, float y) {
    glTranslatef(x, y, 0.0f);
}

void Canvas::fillRect(int x, int y, int width, int height, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glRecti(x, y, x + width, y + height);
    glPopAttrib();
}

void Canvas::fillRoundedRect(int x, int y, int width, int height, int radius, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    radius = clampCornerRadius(width, height, radius);
    const std::vector<int> extents = rowExtents(radius);

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    beginPixels(color);

    for (int row = 0; row < height; row++) {
        const auto [left, right] = roundedRowSpan(width, height, radius, extents, row);
        pixelRect(x + left, y + row, right - left + 1);
    }

    glEnd();
    glPopAttrib();
}

void Canvas::strokeRoundedRect(int x, int y, int width, int height, int radius, int thickness,
                                      const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || thickness <= 0 || color.a <= 0.0f) {
        return;
    }

    radius = clampCornerRadius(width, height, radius);
    const std::vector<int> outerExtents = rowExtents(radius);

    // 內圓角矩形與 fillRoundedRect(x + t, y + t, ...) 使用相同的像素，兩者可無縫拼接。
    const int innerWidth = width - 2 * thickness;
    const int innerHeight = height - 2 * thickness;
    const bool hasInner = innerWidth > 0 && innerHeight > 0;
    const int innerRadius = hasInner ? clampCornerRadius(innerWidth, innerHeight, radius - thickness) : 0;
    const std::vector<int> innerExtents = rowExtents(innerRadius);

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    beginPixels(color);

    for (int row = 0; row < height; row++) {
        const auto [left, right] = roundedRowSpan(width, height, radius, outerExtents, row);
        const int innerRow = row - thickness;

        if (!hasInner || innerRow < 0 || innerRow >= innerHeight) {
            pixelRect(x + left, y + row, right - left + 1);  // 上下邊框整列填滿
            continue;
        }

        // 中間列只畫內圓角矩形左右兩側的區段。
        const auto [innerLeft, innerRight] =
            roundedRowSpan(innerWidth, innerHeight, innerRadius, innerExtents, innerRow);
        pixelRect(x + left, y + row, thickness + innerLeft - left);
        pixelRect(x + thickness + innerRight + 1, y + row, right - (thickness + innerRight));
    }

    glEnd();
    glPopAttrib();
}

}  // namespace robot::graphics
