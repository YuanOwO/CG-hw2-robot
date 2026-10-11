#pragma once

#include <functional>

#include "render/camera.hpp"
#include "ui/element.hpp"

namespace robot::ui {

// 在元素範圍內顯示 3D 場景的元件。
//
// 場景本身由 drawScene 繪製：呼叫時已套用此元件的相機與平行投影，座標為世界座標。
// 元件只決定「場景顯示在哪裡、從哪個角度看」，不擁有也不認識場景的內容。
// 3D 內容畫在背景之後、子元件之前，因此子元件（例如標題文字）會疊在場景上；
// 子元件各自依對齊方式排在內容區域內，彼此重疊。
class ViewportElement : public Element {
   public:
    using DrawScene = std::function<void()>;

    explicit ViewportElement(DrawScene drawScene, const render::Camera& camera = {},
                             const render::OrthoProjection& projection = {});

    const render::Camera& camera() const { return _camera; }
    void setCamera(const render::Camera& camera);

    const render::OrthoProjection& projection() const { return _projection; }
    void setProjection(const render::OrthoProjection& projection);

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;
    void renderContent(render::RenderContext& context) override;

   private:
    DrawScene _drawScene;
    render::Camera _camera;
    render::OrthoProjection _projection;
};

}  // namespace robot::ui
