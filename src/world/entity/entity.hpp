#pragma once

#include <cstdint>

#include "math/random.hpp"
#include "math/vec3.hpp"

namespace robot::world {
class World;
}

namespace robot::world::entity {

using EntityId = std::uint32_t;

class Entity {
   public:
    Entity(EntityId id, std::uint64_t seed, const Vec3& position, float yaw, float pitch = 0.0f,
           float eyeHeight = 0.0f);
    virtual ~Entity() = default;

    // 禁止複製與移動
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = delete;
    Entity& operator=(Entity&&) = delete;

    virtual void tick(float dt, const World& world) = 0;

    EntityId id() const { return _id; }
    const Vec3& position() const { return _position; }
    virtual Vec3 eyePosition() const { return _position + Vec3(0.0f, _eyeHeight, 0.0f); }
    const Vec3& motion() const { return _motion; }
    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }

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
    Vec3 _motion;  // 公尺 / 秒
    float _yaw;    // 度，[0, 360)
    float _pitch;  // 度，[-90, 90]

    float _eyeHeight;  // 公尺，眼睛相對於腳底的高度
};

}  // namespace robot::world::entity
