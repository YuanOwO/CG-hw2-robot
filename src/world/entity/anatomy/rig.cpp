#include "world/entity/anatomy/rig.hpp"

#include <stdexcept>

namespace robot::world::entity::anatomy {

void Rig::animate(const AnimationInput& input, Pose& pose) const {
    for (const auto& part : _parts) {
        part->animate(input, pose);
    }
}

const Head& Rig::addMainHead(BoneIndex parent, const BoneShape& shape, const JointLimits& limits) {
    if (_mainHead) {
        throw std::logic_error("A rig has at most one main head.");
    }
    _mainHead = &add<Head>(parent, shape, limits);
    return *_mainHead;
}

MarkerIndex Rig::addEye(const Vec3& offset) {
    if (!_mainHead) {
        throw std::logic_error("The eye must be on the main head; add it first.");
    }
    if (_eye != NO_MARKER) {
        throw std::logic_error("A rig has at most one eye.");
    }
    _eye = addMarker({"eye", _mainHead->bone(), offset});
    return _eye;
}

}  // namespace robot::world::entity::anatomy
