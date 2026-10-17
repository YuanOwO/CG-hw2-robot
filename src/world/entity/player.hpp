#pragma once

#include <optional>

#include "world/entity/living_entity.hpp"

namespace robot::world::entity {

struct PlayerLook {
    float yaw = 0.0f;    // 度，想看的方向；玩家會轉過去，不會瞬間到位
    float pitch = 0.0f;  // 度，正值抬頭
};

// 這個 tick 玩家想做什麼。移動量相對於視線方向，與輸入裝置無關
struct PlayerCommand {
    float forward = 0.0f;            // -1～1，正值往前
    float strafe = 0.0f;             // -1～1，正值往右
    std::optional<PlayerLook> look;  // 沒有時維持原本的視線
};

class Player : public LivingEntity {
   public:
    Player(EntityId id, std::uint64_t seed, const Vec3& position, float yaw);

    static const anatomy::Rig& speciesRig();

    void setCommand(const PlayerCommand& command) { _command = command; }

   protected:
    MoveInput aiStep(float dt, const World& world) override;

   private:
    PlayerCommand _command;
};

}  // namespace robot::world::entity
