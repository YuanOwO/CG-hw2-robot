#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include "math/vec3.hpp"
#include "world/anatomy/builder.hpp"
#include "world/anatomy/parts/head.hpp"
#include "world/anatomy/parts/leg.hpp"
#include "world/anatomy/parts/neck.hpp"
#include "world/anatomy/parts/torso.hpp"
#include "world/anatomy/parts/wing.hpp"
#include "world/anatomy/rig.hpp"
#include "world/anatomy/types.hpp"
#include "world/body_plans/body_plan.hpp"

namespace robot::world::body_plans {

using anatomy::Side;

struct BirdSpec {
    anatomy::BipedTorsoSpec torso;
    std::optional<anatomy::NeckSpec> neck;
    anatomy::HeadSpec head;
    anatomy::WingSpec wing;
    anatomy::LegSpec leg;
};

struct Bird : public BodyPlan {
    Vec3 rootOffset;

    anatomy::BipedTorso torso;
    std::optional<anatomy::Neck> neck;
    anatomy::Head head;
    anatomy::Wing leftWing;
    anatomy::Wing rightWing;
    anatomy::Leg leftLeg;
    anatomy::Leg rightLeg;
    anatomy::Rig rig;

    Bird(const std::string& name, const BirdSpec& spec)
        : rootOffset(Vec3(0.0f, spec.leg.length(), 0.0f)),
          torso(name + "_torso", spec.torso),
          neck(spec.neck.has_value() ? std::make_optional<anatomy::Neck>(name + "_neck", spec.neck.value())
                                     : std::nullopt),
          head(name + "_head", spec.head),
          leftWing(name + "_left_wing", Side::Left, spec.wing),
          rightWing(name + "_right_wing", Side::Right, spec.wing),
          leftLeg(name + "_left_leg", Side::Left, spec.leg),
          rightLeg(name + "_right_leg", Side::Right, spec.leg),
          rig(assemble()) {
        requireAttached({&torso, &head, &leftWing, &rightWing, &leftLeg, &rightLeg});

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
            builder.addPart(neck.value().headSocket(), head);
        } else {
            builder.addPart(torso.neck(), head);
        }

        builder.addPart(torso.shoulder(Side::Left), leftWing);
        builder.addPart(torso.shoulder(Side::Right), rightWing);
        builder.addPart(torso.hip(Side::Left), leftLeg);
        builder.addPart(torso.hip(Side::Right), rightLeg);
        return std::move(builder).build();
    }
};

}  // namespace robot::world::body_plans
