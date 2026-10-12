#pragma once

#include "graphics/mesh.hpp"
#include "world/entity/creeper.hpp"

namespace robot::view::entity {

// 所有苦力怕共用同一份網格，各自只在繪製時套用位置與朝向。
class CreeperRenderer {
   public:
    CreeperRenderer();

    // 呼叫前 MODELVIEW 須為世界座標系。
    void render(const world::entity::Creeper& creeper) const;

   private:
    // 每個網格建在它所屬關節的座標系裡
    graphics::Mesh _body;  // 根：腳底中心
    graphics::Mesh _head;  // 脖子：頭的底面中心
    graphics::Mesh _face;  // 脖子，與頭相同
    graphics::Mesh _leg;   // 腿頂中心，四條腿共用

    void drawHead(float pitch) const;
    void drawLegs(float swing) const;
};

}  // namespace robot::view::entity
