#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "math/random.hpp"
#include "world/entity/entity.hpp"

namespace robot::world::entity {

// 保存世界中所有的實體，並分配 id 與 seed。
// 存 unique_ptr：Entity 是抽象類別且禁止複製，不能直接放進 vector；spawn 回傳的參考也不會因 vector 擴充而失效。
class EntityManager {
   public:
    explicit EntityManager(std::uint64_t seed);

    // id 與 seed 接在其餘參數前面：spawn<Creeper>(position, yaw) 建構 Creeper(id, seed, position, yaw)
    template <typename T, typename... Args>
    T& spawn(Args&&... args) {
        const EntityId id = _nextEntityId++;
        auto entity = std::make_unique<T>(id, seedFor(id), std::forward<Args>(args)...);
        T& reference = *entity;
        _entities.push_back(std::move(entity));
        return reference;
    }

    void tick(float dt, const World& world);

    // 依生成順序
    template <typename F>
    void forEach(F&& visit) const {
        for (const auto& entity : _entities) {
            visit(*entity);
        }
    }

    // 只走訪型別為 T 或其子類別的實體
    template <typename T, typename F>
    void forEach(F&& visit) const {
        for (const auto& entity : _entities) {
            if (const T* typed = dynamic_cast<const T*>(entity.get())) {
                visit(*typed);
            }
        }
    }

   private:
    Random _random;
    std::vector<std::unique_ptr<Entity>> _entities;
    EntityId _nextEntityId = 1;  // 0 保留給「沒有實體」

    std::uint64_t seedFor(EntityId id) const;
};

}  // namespace robot::world::entity
