#include "ui/elements/viewport.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "render/scene_3d_scope.hpp"

namespace robot::ui {

ViewportElement::ViewportElement(DrawScene drawScene, const render::Camera& camera,
                                 const render::OrthoProjection& projection)
    : _drawScene(std::move(drawScene)), _camera(camera), _projection(projection) {}

void ViewportElement::setCamera(const render::Camera& camera) {
    _camera = camera;
    invalidateDisplay();
}

void ViewportElement::setProjection(const render::OrthoProjection& projection) {
    _projection = projection;
    invalidateDisplay();
}

Size ViewportElement::measureContent(const Size& availableSize) {
    // 場景本身沒有固定大小，需求大小只取決於疊在上面的子元件。
    int width = 0;
    int height = 0;
    for (const auto& child : children()) {
        const Size desired = child->measure(availableSize);
        width = std::max(width, desired.width.value_or(0));
        height = std::max(height, desired.height.value_or(0));
    }
    return {width, height};
}

void ViewportElement::arrangeContent(const BoundingBox& contentBounds) {
    for (const auto& child : children()) {
        child->arrange(contentBounds);
    }
}

void ViewportElement::renderContent(render::RenderContext& context) {
    if (!_drawScene) {
        return;
    }

    // Scene3DScope 使用視窗座標；元素的 bounds 相對於父元素，需先換算。
    const Point origin = localToWindow({0.0f, 0.0f});
    const BoundingBox area(static_cast<int>(std::lround(origin.x)), static_cast<int>(std::lround(origin.y)), width(),
                           height());

    render::Scene3DScope scope(context, area, _camera, _projection);
    if (scope) {
        _drawScene();
    }
}

}  // namespace robot::ui
