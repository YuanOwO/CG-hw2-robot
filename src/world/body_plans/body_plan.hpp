#pragma once

#include <initializer_list>
#include <optional>
#include <stdexcept>

#include "world/anatomy/parts/part.hpp"

namespace robot::world::body_plans {

struct BodyPlan {
    virtual ~BodyPlan() = default;

    void requireAttached(std::initializer_list<anatomy::Part*> parts) const {
        for (const auto part : parts) {
            if (part->state() != anatomy::Part::AttachState::Attached) {
                throw std::logic_error("Part is not attached: " + part->name());
            }
        }
    }
};

}  // namespace robot::world::body_plans
