#include "world/collision/collision.hpp"

namespace robot::world::collision {

#pragma region Box

Box::Box(const Vec3& center, const Vec3& halfExtents) : _center(center), _halfExtents(halfExtents) {
    if (_halfExtents.x < 0.0f || _halfExtents.y < 0.0f || _halfExtents.z < 0.0f) {
        throw std::invalid_argument("Half extents must be non-negative");
    }
}

std::array<Vec3, 8> Box::corners() const {
    std::array<Vec3, 8> corners;
    for (int i = 0; i < 8; ++i) {
        float dx = (i & 1) ? _halfExtents.x : -_halfExtents.x;
        float dy = (i & 2) ? _halfExtents.y : -_halfExtents.y;
        float dz = (i & 4) ? _halfExtents.z : -_halfExtents.z;
        corners[i] = _center + Vec3(dx, dy, dz);
    }
    return corners;
}

std::array<Vec3, 8> Box::transformedCorners(const Mat4& transform) const {
    auto result = corners();

    for (auto& corner : result) {
        corner = transformPoint(transform, corner);
    }

    return result;
}

#pragma endregion  // Box

#pragma region UprightCylinder

UprightCylinder::UprightCylinder(const Vec3& baseCenter, float radius, float height)
    : _baseCenter(baseCenter), _radius(radius), _height(height) {
    if (_radius < 0.0f || _height < 0.0f) {
        throw std::invalid_argument("Radius and height must be non-negative");
    }
}

UprightCylinder UprightCylinder::translated(const Vec3& offset) const {
    return UprightCylinder(_baseCenter + offset, _radius, _height);
}

bool UprightCylinder::overlaps(const UprightCylinder& other) const {
    // 檢查垂直方向的重疊
    if (topY() < other.bottomY() || bottomY() > other.topY()) {
        return false;
    }

    // 檢查水平方向的重疊（圓柱底面投影為圓形）
    float dx = _baseCenter.x - other._baseCenter.x;
    float dz = _baseCenter.z - other._baseCenter.z;

    float radiusSum = _radius + other._radius;

    return dx * dx + dz * dz <= radiusSum * radiusSum;
}

#pragma endregion  // UprightCylinder

}  // namespace robot::world::collision
