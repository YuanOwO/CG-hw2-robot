#pragma once

#include <array>

#include "math/mat4.hpp"
#include "math/vec3.hpp"

namespace robot::world::collision {

class Box {
   public:
    Box(const Vec3& center, const Vec3& halfExtents);
    ~Box() = default;

    const Vec3& center() const { return _center; }  ///< 取得碰撞箱的中心點座標

    const Vec3& halfExtents() const { return _halfExtents; }  ///< 取得碰撞箱的半尺寸

    std::array<Vec3, 8> corners() const;

    std::array<Vec3, 8> transformedCorners(const Mat4& transform) const;

   private:
    Vec3 _center;
    Vec3 _halfExtents;
};

class UprightCylinder {
   public:
    UprightCylinder(const Vec3& baseCenter, float radius, float height);
    ~UprightCylinder() = default;

    const Vec3& baseCenter() const { return _baseCenter; }

    float radius() const { return _radius; }

    float height() const { return _height; }

    float bottomY() const { return _baseCenter.y; }

    float topY() const { return _baseCenter.y + _height; }

    UprightCylinder translated(const Vec3& offset) const;

    bool overlaps(const UprightCylinder& other) const;

   private:
    Vec3 _baseCenter;
    float _radius;
    float _height;
};

}  // namespace robot::world::collision
