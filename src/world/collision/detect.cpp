#include "world/collision/detect.hpp"

#include <algorithm>
#include <cmath>
#include <variant>

#include "math/constants.hpp"

namespace robot::world::collision {

std::optional<Contact> detect(const UprightCylinder& player, const UprightBox& obstacle) {
    // 先檢查垂直方向上是否有重疊
    if (player.topY() < obstacle.bottomY() || player.bottomY() > obstacle.topY()) {
        return std::nullopt;  // 沒有垂直重疊，沒有碰撞
    }

    // 檢查水平方向的距離
    Vec3 d = player.baseCenter() - obstacle.baseCenter();
    d = rotateAroundY(d, -obstacle.yaw());  // 將玩家座標轉到障礙物的局部座標系
    Vec3 halfSize = obstacle.size() * 0.5f;

    float closestX = std::clamp(d.x, -halfSize.x, halfSize.x);
    float closestZ = std::clamp(d.z, -halfSize.z, halfSize.z);
    float dx = closestX - d.x;
    float dz = closestZ - d.z;
    float distance = std::hypot(dx, dz);

    if (distance > player.radius()) {
        return std::nullopt;  // 沒有水平重疊，沒有碰撞
    }

    // 計算碰撞法線和深度
    Contact contact;

    if (distance > EPSILON) {
        // 圓心在方形外部
        contact.normal = Vec3(dx / distance, 0.0f, dz / distance);
        contact.depth = player.radius() - distance;
    } else {
        // 圓心在方形內部
        const float toEdgeX = halfSize.x - std::abs(d.x);
        const float toEdgeZ = halfSize.z - std::abs(d.z);
        if (toEdgeX <= toEdgeZ) {
            float signX = d.x >= 0.0f ? -1.0f : 1.0f;
            contact.normal = Vec3(signX, 0.0f, 0.0f);
            contact.depth = toEdgeX + player.radius();
        } else {
            float signZ = d.z >= 0.0f ? -1.0f : 1.0f;
            contact.normal = Vec3(0.0f, 0.0f, signZ);
            contact.depth = toEdgeZ + player.radius();
        }
    }

    contact.normal = rotateAroundY(contact.normal, obstacle.yaw());  // 將法線轉回世界座標系

    // 判斷垂直方向的重疊深度
    const float down = player.topY() - obstacle.bottomY();  // 玩家在障礙物下方
    const float up = obstacle.topY() - player.bottomY();    // 玩家在障礙物上方

    if (down <= up && down <= contact.depth) {
        // 垂直方向的重疊較小，使用垂直方向的法線
        return std::make_optional(Contact{Vec3(0.0f, 1.0f, 0.0f), down});
    }

    if (up <= down && up <= contact.depth) {
        // 垂直方向的重疊較小，使用垂直方向的法線
        return std::make_optional(Contact{Vec3(0.0f, -1.0f, 0.0f), up});
    }

    contact.normal = normalize(contact.normal);  // 確保法線是單位向量

    return std::make_optional(contact);
}

std::optional<Contact> detect(const UprightCylinder& player, const UprightCylinder& obstacle) {
    const float bottom = std::max(player.bottomY(), obstacle.bottomY());
    const float top = std::min(player.topY(), obstacle.topY());
    const float verticalDepth = top - bottom;

    if (verticalDepth < 0.0f) {
        return std::nullopt;  // 沒有垂直重疊，沒有碰撞
    }

    Vec3 d = obstacle.baseCenter() - player.baseCenter();
    const float rho = std::hypot(d.x, d.z);
    const float horizontalDepth = player.radius() + obstacle.radius() - rho;

    if (horizontalDepth < 0.0f) {
        return std::nullopt;  // 沒有水平重疊，沒有碰撞
    }

    // 計算碰撞法線和深度
    Contact contact;

    if (horizontalDepth <= verticalDepth) {
        // 水平方向的重疊較小，使用水平方向的法線
        contact.normal = rho > EPSILON ? Vec3(d.x / rho, 0.0f, d.z / rho) : Vec3(1.0f, 0.0f, 0.0f);
        contact.depth = horizontalDepth;
    } else {
        // 垂直方向的重疊較小，使用垂直方向的法線
        if (player.bottomY() + player.height() * 0.5f < obstacle.bottomY() + obstacle.height() * 0.5f) {
            contact.normal = Vec3(0.0f, 1.0f, 0.0f);  // 玩家在障礙物下方
        } else {
            contact.normal = Vec3(0.0f, -1.0f, 0.0f);  // 玩家在障礙物上方
        }
        contact.depth = verticalDepth;
    }

    contact.normal = normalize(contact.normal);  // 確保法線是單位向量

    return std::make_optional(contact);
}

std::optional<Contact> detect(const UprightCylinder& player, const Sphere& obstacle) {
    // 先檢查垂直方向上是否有重疊
    if (player.topY() < obstacle.center().y - obstacle.radius() ||
        player.bottomY() > obstacle.center().y + obstacle.radius()) {
        return std::nullopt;  // 沒有垂直重疊，沒有碰撞
    }

    // 找圓柱上到球心的最近點
    Vec3 d = obstacle.center() - player.baseCenter();
    const float rho = std::hypot(d.x, d.z);

    Vec3 closestPoint = player.baseCenter();  // 先取圓柱的 x, z 座標
    closestPoint.y = std::clamp(obstacle.center().y, player.bottomY(), player.topY());

    const float scale = rho >= player.radius() ? player.radius() / rho : 1.0f;

    closestPoint.x += scale * d.x;
    closestPoint.z += scale * d.z;

    const Vec3 delta = obstacle.center() - closestPoint;
    const float distance = length(delta);

    if (distance > obstacle.radius()) {
        return std::nullopt;  // 沒有碰撞
    }

    // 計算碰撞法線和深度
    Contact contact;

    if (distance > EPSILON) {
        // 球體在圓柱外部
        contact.normal = delta;
        contact.depth = obstacle.radius() - distance;
    } else {
        // 球體在圓柱內部
        const float side = player.radius() - rho;
        const float top = player.topY() - obstacle.center().y;
        const float bottom = obstacle.center().y - player.bottomY();

        if (side <= top && side <= bottom) {
            // 球體離側面最近
            contact.normal = rho > EPSILON ? Vec3(d.x / rho, 0.0f, d.z / rho) : Vec3(0.0f, 0.0f, 1.0f);
            contact.depth = side + obstacle.radius();
        } else if (top <= bottom) {
            // 球體離上表面最近
            contact.normal = Vec3(0.0f, 1.0f, 0.0f);
            contact.depth = top + obstacle.radius();
        } else {
            // 球體離下表面最近
            contact.normal = Vec3(0.0f, -1.0f, 0.0f);
            contact.depth = bottom + obstacle.radius();
        }
    }

    contact.normal = normalize(contact.normal);  // 確保法線是單位向量

    return std::make_optional(contact);
}

std::optional<Contact> detect(const UprightCylinder& player, const Collider& obstacle) {
    return std::visit([&](const auto& c) { return detect(player, c); }, obstacle);
}

}  // namespace robot::world::collision
