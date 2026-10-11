#pragma once

#include "common/bounding_box.hpp"
#include "render/camera.hpp"
#include "render/render_context.hpp"

namespace robot::render {

// 在 2D 介面的繪製過程中，暫時切換到 3D 繪製狀態的 RAII 物件。
//
// 建構時保存目前的 OpenGL 狀態，再把繪製限制在 area（視窗座標，左上為原點、y 向下）內：
// 清除該區域的深度、套用平行投影與相機、開啟深度測試。解構時恢復所有保存的狀態，
// 介面可以接著照常繪製，因此 3D 內容會依介面樹的順序與其他元件互相覆蓋。
//
// 不支援巢狀使用：scope 存在期間不應再建立另一個 scope。
class Scene3DScope {
   public:
    Scene3DScope(RenderContext& context, const BoundingBox& area, const Camera& camera,
                 const OrthoProjection& projection);
    ~Scene3DScope();

    Scene3DScope(const Scene3DScope&) = delete;
    Scene3DScope& operator=(const Scene3DScope&) = delete;
    Scene3DScope(Scene3DScope&&) = delete;
    Scene3DScope& operator=(Scene3DScope&&) = delete;

    // area 為空時不會切換狀態，回傳 false，呼叫端應跳過繪製。
    explicit operator bool() const { return _active; }

   private:
    bool _active = false;
};

}  // namespace robot::render
