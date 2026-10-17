#include "graphics/camera.hpp"

namespace robot::graphics {

OrthoProjection OrthoProjection::fittedTo(float aspect) const {
    const float centerX = (left + right) / 2.0f;
    const float centerY = (bottom + top) / 2.0f;
    float halfWidth = (right - left) / 2.0f;
    float halfHeight = (top - bottom) / 2.0f;

    if (aspect > halfWidth / halfHeight) {
        halfWidth = halfHeight * aspect;  // 繪圖區域比 view volume 寬：放寬左右
    } else {
        halfHeight = halfWidth / aspect;  // 繪圖區域比 view volume 高：放寬上下
    }

    OrthoProjection fitted = *this;
    fitted.left = centerX - halfWidth;
    fitted.right = centerX + halfWidth;
    fitted.bottom = centerY - halfHeight;
    fitted.top = centerY + halfHeight;
    return fitted;
}

}  // namespace robot::graphics
