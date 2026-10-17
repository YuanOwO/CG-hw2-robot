#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "world/entity/anatomy/parts.hpp"
#include "world/entity/anatomy/skeleton.hpp"

namespace robot::world::entity::anatomy {

// 一種生物的身體：由部件組成的骨架。同種實體共用一份，各自的姿態放在 Pose。
// 子類別只在建構子裡組裝部件，動作由各部件負責
class Rig {
   public:
    virtual ~Rig() = default;

    // 部件與實體都記著骨頭編號，Rig 不能被複製或移動
    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;
    Rig(Rig&&) = delete;
    Rig& operator=(Rig&&) = delete;

    const Skeleton& skeleton() const { return _skeleton; }
    const Head* mainHead() const { return _mainHead; }  // 沒有主頭時為 nullptr
    MarkerIndex eye() const { return _eye; }            // 在主頭上；沒有時為 NO_MARKER

    // 身體與視線最大的夾角，度；沒有主頭時身體直接朝向視線
    float maxHeadYaw() const { return _mainHead ? _mainHead->limits().yaw : 0.0f; }

    // 依加入的順序驅動每個部件。特殊的生物可以 override，先呼叫這個再補上自己的動作
    virtual void animate(const AnimationInput& input, Pose& pose) const;

   protected:
    Rig() = default;

    // 建立部件並加入骨架；parent 必須已經存在
    template <typename T, typename... Args>
    T& add(BoneIndex parent, Args&&... args) {
        static_assert(std::is_base_of_v<Part, T>, "Rig can only add parts.");

        auto part = std::make_unique<T>(_skeleton, parent, std::forward<Args>(args)...);
        T& ref = *part;
        _parts.push_back(std::move(part));
        return ref;
    }

    // 主頭決定視線：身體與視線的最大夾角、眼睛的位置。最多一顆；其他的頭用 add<Head>
    const Head& addMainHead(BoneIndex parent, const BoneShape& shape, const JointLimits& limits = Head::DEFAULT_LIMITS);

    MarkerIndex addMarker(const Marker& marker) { return _skeleton.addMarker(marker); }
    // 眼睛一定在主頭上，offset 相對於主頭
    MarkerIndex addEye(const Vec3& offset);

   private:
    Skeleton _skeleton;
    std::vector<std::unique_ptr<Part>> _parts;
    const Head* _mainHead = nullptr;  // 由 _parts 持有
    MarkerIndex _eye = NO_MARKER;
};

}  // namespace robot::world::entity::anatomy
