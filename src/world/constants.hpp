#pragma once

namespace robot::world {

constexpr float GRAVITY = 9.80665f;  ///< 地球表面重力加速度，單位 m/s²

constexpr float GROUND_SNAP_DISTANCE = 0.3f;  ///< 腳底距離地面小於此值時，會被視為站在地面上

}  // namespace robot::world
