#pragma once

namespace robot::world {

// 世界的狀態與規則，不涉及繪製與輸入。目前是一塊繞中心旋轉的地板。
class World {
   public:
    void tick(float dt);

    float floorAngle() const { return _floorAngle; }

   private:
    float _floorAngle = 0.0f;  // 度
};

}  // namespace robot::world
