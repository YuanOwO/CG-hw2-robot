#pragma once

#include <optional>
#include <string>

#include "geometry/mesh/mesh.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/joint.hpp"
#include "world/anatomy/types.hpp"
#include "world/collision/collision.hpp"

namespace robot::world::anatomy {

struct BoneSpec {
    Joint joint;

    std::optional<collision::Box> collider;
    std::optional<geometry::Mesh> mesh;
};

class Bone {
   public:
    Bone(const std::string& name, BoneIndex parent, const Vec3& pivot, BoneSpec spec)
        : _name(name), _parent(parent), _pivot(pivot), _joint(spec.joint), _collider(spec.collider), _mesh(spec.mesh) {}
    ~Bone() = default;

    const std::string& name() const { return _name; }
    BoneIndex parent() const { return _parent; }
    const Vec3& pivot() const { return _pivot; }
    const Joint& joint() const { return _joint; }

    const std::optional<collision::Box>& collider() const { return _collider; }
    const std::optional<geometry::Mesh>& mesh() const { return _mesh; }

   private:
    std::string _name;
    BoneIndex _parent;
    Vec3 _pivot;
    Joint _joint;

    std::optional<collision::Box> _collider;
    std::optional<geometry::Mesh> _mesh;
};

struct Marker {
    std::string name;
    BoneIndex bone;
    Vec3 offset;
};

}  // namespace robot::world::anatomy
