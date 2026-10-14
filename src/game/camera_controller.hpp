#pragma once

#include <optional>

#include "common/point.hpp"
#include "engine/input.hpp"
#include "view/camera_rig.hpp"

namespace robot::game {

// 把按鍵與滑鼠拖曳轉成相機的轉動與縮放；CameraRig 本身只管幾何
class CameraController {
   public:
    void update(float dt, const engine::Input& input, view::CameraRig& camera);
    // 停止 update 時呼叫：期間游標的位移不屬於這次拖曳，恢復時要重新記錄起點
    void reset() { _dragFrom.reset(); }

   private:
    // 拖曳視角時上一個 tick 的游標位置；沒有在拖曳時為空，按下的那一刻才記錄起點
    std::optional<Point> _dragFrom;
};

}  // namespace robot::game
