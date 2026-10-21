#pragma once

#include <cmath>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include "math/constants.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/builder.hpp"
#include "world/anatomy/parts/head.hpp"
#include "world/anatomy/parts/leg.hpp"
#include "world/anatomy/parts/neck.hpp"
#include "world/anatomy/parts/torso.hpp"
#include "world/anatomy/rig.hpp"
#include "world/anatomy/types.hpp"
#include "world/body_plans/body_plan.hpp"

namespace robot::world::body_plans {

using anatomy::Side;

struct QuadrupedSpec {
    anatomy::QuadrupedTorsoSpec torso;
    std::optional<anatomy::NeckSpec> neck;
    anatomy::HeadSpec head;
    anatomy::LegSpec foreleg;
    anatomy::LegSpec hindleg;
};

struct Quadruped : public BodyPlan {
    Vec3 rootOffset;

    anatomy::QuadrupedTorso torso;
    std::optional<anatomy::Neck> neck;
    anatomy::Head head;
    anatomy::Leg leftForeleg;
    anatomy::Leg rightForeleg;
    anatomy::Leg leftHindleg;
    anatomy::Leg rightHindleg;
    anatomy::Rig rig;

    Quadruped(const std::string& name, const QuadrupedSpec& spec)
        : rootOffset(Vec3(0.0f, spec.foreleg.length(), 0.0f)),
          torso(name + "_torso", spec.torso),
          neck(spec.neck.has_value() ? std::make_optional<anatomy::Neck>(name + "_neck", spec.neck.value())
                                     : std::nullopt),
          head(name + "_head", spec.head),
          leftForeleg(name + "_left_foreleg", Side::Left, spec.foreleg),
          rightForeleg(name + "_right_foreleg", Side::Right, spec.foreleg),
          leftHindleg(name + "_left_hindleg", Side::Left, spec.hindleg),
          rightHindleg(name + "_right_hindleg", Side::Right, spec.hindleg),
          rig(assemble()) {
        if (std::abs(spec.foreleg.length() - spec.hindleg.length()) > EPSILON) {
            throw std::invalid_argument("Foreleg and hindleg total lengths must be equal");
        }

        requireAttached({&torso, &head, &leftForeleg, &rightForeleg, &leftHindleg, &rightHindleg});

        if (neck.has_value() && neck->state() != anatomy::Part::AttachState::Attached) {
            throw std::runtime_error("Failed to attach part: " + neck->name());
        }
    }

   private:
    anatomy::Rig assemble() {
        anatomy::RigBuilder builder(rootOffset);
        builder.addPart(builder.root(), torso);

        if (neck.has_value()) {
            builder.addPart(torso.neck(), neck.value());
            builder.addPart(neck->headSocket(), head);
        } else {
            builder.addPart(torso.neck(), head);
        }

        builder.addPart(torso.shoulder(Side::Left), leftForeleg);
        builder.addPart(torso.shoulder(Side::Right), rightForeleg);
        builder.addPart(torso.hip(Side::Left), leftHindleg);
        builder.addPart(torso.hip(Side::Right), rightHindleg);
        return std::move(builder).build();
    }
};

}  // namespace robot::world::body_plans
