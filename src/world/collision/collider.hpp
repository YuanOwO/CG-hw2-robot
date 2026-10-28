#pragma once

#include <variant>

#include "math/vec3.hpp"

namespace robot::world::collision {

class UprightBox {
   public:
    UprightBox(const Vec3& baseCenter, const Vec3& size, float yaw);
    ~UprightBox() = default;

    const Vec3& baseCenter() const { return _baseCenter; }

    float bottomY() const { return _baseCenter.y; }
    float topY() const { return _baseCenter.y + _size.y; }

    const Vec3& size() const { return _size; }
    float yaw() const { return _yaw; }

    UprightBox translated(const Vec3& translation) const;

   private:
    Vec3 _baseCenter;
    Vec3 _size;
    float _yaw;
};

class UprightCylinder {
   public:
    UprightCylinder(const Vec3& baseCenter, float radius, float height);
    ~UprightCylinder() = default;

    const Vec3& baseCenter() const { return _baseCenter; }

    float bottomY() const { return _baseCenter.y; }
    float topY() const { return _baseCenter.y + _height; }

    float radius() const { return _radius; }
    float height() const { return _height; }

    UprightCylinder translated(const Vec3& translation) const;

   private:
    Vec3 _baseCenter;
    float _radius;
    float _height;
};

class Sphere {
   public:
    Sphere(const Vec3& center, float radius);
    ~Sphere() = default;

    const Vec3& center() const { return _center; }
    float radius() const { return _radius; }

    float bottomY() const { return _center.y - _radius; }
    float topY() const { return _center.y + _radius; }

    Sphere translated(const Vec3& translation) const;

   private:
    Vec3 _center;
    float _radius;
};

using Collider = std::variant<UprightBox, UprightCylinder, Sphere>;

Collider translated(const Collider& collider, const Vec3& translation);

}  // namespace robot::world::collision
