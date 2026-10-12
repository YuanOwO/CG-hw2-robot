#pragma once

#include <cstdint>

#include "math/random.hpp"
#include "math/vec3.hpp"

namespace robot::world {
class World;
}

namespace robot::world::entity {

using EntityId = std::uint32_t;

// 水平的前方單位向量，yaw 的慣例與 Entity 相同
Vec3 forwardOf(float yaw);

// 世界中會動的東西。可以自由跨越 chunk 邊界，因此由 World 統一保存，不屬於任何 chunk。
// 狀態只能透過 setter 修改，yaw 與 pitch 才能一直維持在各自的範圍內。
//
// 身體永遠直立，只有 yaw 會轉動身體；pitch 是視線的仰角，由繪製端套到頭部。
// 沒有 roll：側傾只出現在動畫（走路搖擺、斜坡傾斜），由繪製端依狀態算出，不屬於實體的狀態。
class Entity {
   public:
    // id 與 seed 由 EntityManager 分配：id 在同一個世界內不重複，seed 由世界 seed 與 id 衍生。
    Entity(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float pitch = 0.0f);
    virtual ~Entity() = default;

    // 透過基底類別複製只會複製到 Entity 的部分，子類別的狀態全部遺失
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // World 唯讀：實體只能查詢地形與障礙物，貼地等規則由 World 在 tick 之後統一套用
    virtual void tick(float dt, const World& world) = 0;

    EntityId id() const { return _id; }
    const Vec3& position() const { return _position; }
    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }
    const Vec3& motion() const { return _motion; }

    void setGroundHeight(float height) { _position.y = height; }

   protected:
    // 各實體獨立的亂數來源，同一個世界裡同一個 id 每次都相同
    const Random& random() const { return _random; }

    void setPosition(const Vec3& position) { _position = position; }
    void setYaw(float yaw);
    void setPitch(float pitch);
    void setMotion(const Vec3& motion) { _motion = motion; }

    void move(float dt) { _position += _motion * dt; }

   private:
    const EntityId _id;
    Random _random;
    Vec3 _position;
    float _yaw;    // 度，[0, 360)；0 面向 +z，從上方看逆時針為正
    float _pitch;  // 度，[-90, 90]；0 平視，正值抬頭。超過 ±90 會變成往後仰翻過去，因此夾住而不是繞回
    Vec3 _motion;  // 公尺 / 秒
};

}  // namespace robot::world::entity
