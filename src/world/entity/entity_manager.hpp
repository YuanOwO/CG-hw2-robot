#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "world/entity/entity.hpp"
#include "world/types.hpp"
#include "world/utils.hpp"

namespace robot::world::entity {

// 保存世界中所有的實體，並分配 id 與 seed。
// 存 unique_ptr：Entity 是抽象類別且禁止複製，不能直接放進 vector；spawn 回傳的參考也不會因 vector 擴充而失效。
class EntityManager {
   public:
    explicit EntityManager(Seed seed);

    void tick(float dt, const World& world);

    template <typename T, typename... Args>
    T& spawn(Args&&... args) {
        const EntityId id = _nextEntityId++;
        Seed entitySeed = deriveSeed(_seed, ENTITY_SEED_STREAM, id);
        auto entity = std::make_unique<T>(id, entitySeed, std::forward<Args>(args)...);
        T& ref = *entity;
        _entities.push_back(std::move(entity));
        return ref;
    }

    // 依生成順序
    template <typename Func>
    void forEach(Func&& visit) const {
        for (const auto& entity : _entities) {
            visit(*entity);
        }
    }

    // 只走訪型別為 T 或其子類別的實體
    template <typename T, typename Func>
    void forEach(Func&& visit) const {
        for (const auto& entity : _entities) {
            if (const T* typed = dynamic_cast<const T*>(entity.get())) {
                visit(*typed);
            }
        }
    }

   private:
    Seed _seed;
    std::vector<std::unique_ptr<Entity>> _entities;
    EntityId _nextEntityId = 1;  // 0 保留給 "沒有實體"
};

}  // namespace robot::world::entity
