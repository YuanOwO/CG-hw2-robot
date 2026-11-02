#pragma once

#include <optional>

#include "common/point.hpp"
#include "engine/input.hpp"

namespace robot::game {

class CameraController {
   public:
    struct CameraTurn {
        float yaw = 0.0f;    ///< 度，正值逆時針
        float pitch = 0.0f;  ///< 度，正值抬頭
        float zoom = 1.0f;   ///< 倍數，>1 放大，<1 縮小
    };

    CameraTurn update(float dt, const engine::Input& input);
    // 停止 update 時呼叫：期間游標的位移不屬於這次拖曳，恢復時要重新記錄起點
    void reset() { _dragFrom.reset(); }

   private:
    // 拖曳視角時上一個 tick 的游標位置；沒有在拖曳時為空，按下的那一刻才記錄起點
    std::optional<Point> _dragFrom;
};

}  // namespace robot::game
