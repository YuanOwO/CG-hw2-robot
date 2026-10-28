#include "world/collision/collider.hpp"

#include <stdexcept>

#include "math/utils.hpp"

namespace robot::world::collision {

UprightBox::UprightBox(const Vec3& baseCenter, const Vec3& size, float yaw)
    : _baseCenter(baseCenter), _size(size), _yaw(wrapDegrees(yaw)) {
    if (_size.x < 0.0f || _size.y < 0.0f || _size.z < 0.0f) {
        throw std::invalid_argument("Size components must be non-negative");
    }
}

UprightBox UprightBox::translated(const Vec3& translation) const {
    Vec3 newBaseCenter = _baseCenter + translation;
    return UprightBox(newBaseCenter, _size, _yaw);
}

UprightCylinder::UprightCylinder(const Vec3& baseCenter, float radius, float height)
    : _baseCenter(baseCenter), _radius(radius), _height(height) {
    if (_radius < 0.0f || _height < 0.0f) {
        throw std::invalid_argument("Radius and height must be non-negative");
    }
}

UprightCylinder UprightCylinder::translated(const Vec3& translation) const {
    Vec3 newBaseCenter = _baseCenter + translation;
    return UprightCylinder(newBaseCenter, _radius, _height);
}

Sphere::Sphere(const Vec3& center, float radius) : _center(center), _radius(radius) {
    if (_radius < 0.0f) {
        throw std::invalid_argument("Radius must be non-negative");
    }
}

Sphere Sphere::translated(const Vec3& translation) const {
    Vec3 newCenter = _center + translation;
    return Sphere(newCenter, _radius);
}

Collider translated(const Collider& collider, const Vec3& translation) {
    return std::visit([&](const auto& c) -> Collider { return c.translated(translation); }, collider);
}

}  // namespace robot::world::collision
