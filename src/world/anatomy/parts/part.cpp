#include "world/anatomy/parts/part.hpp"

#include <stdexcept>

#include "world/anatomy/builder.hpp"

namespace robot::world::anatomy {

void Part::attachTo(RigBuilder& builder, Socket socket) {
    if (_state != AttachState::Detached) {
        throw std::logic_error("Part cannot be attached again");
    }
    try {
        onAttach(builder, socket);
        _state = AttachState::Attached;
    } catch (...) {
        _state = AttachState::Failed;
        throw;
    }
}

void Part::requireAttached() const {
    if (_state != AttachState::Attached) {
        throw std::logic_error("Part is not attached");
    }
}

}  // namespace robot::world::anatomy
