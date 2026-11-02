#pragma once

#include <utility>
#include <variant>

#include "geometry/curve/arc.hpp"
#include "geometry/curve/bezier.hpp"
#include "geometry/curve/line_segment.hpp"

namespace robot::geometry {

class Curve2D {
   public:
    Curve2D(const LineSegment2D& curve) : _curve(curve) {}
    Curve2D(const Arc2D& curve) : _curve(curve) {}
    Curve2D(const QuadraticBezier2D& curve) : _curve(curve) {}
    Curve2D(const CubicBezier2D& curve) : _curve(curve) {}

    ~Curve2D() = default;

    Vec2 startPoint() const {
        return std::visit([](const auto& curve) { return curve.startPoint(); }, _curve);
    }

    Vec2 endPoint() const {
        return std::visit([](const auto& curve) { return curve.endPoint(); }, _curve);
    }

    Vec2 evaluate(float t) const {
        return std::visit([t](const auto& curve) { return curve.evaluate(t); }, _curve);
    }

    Vec2 derivative(float t) const {
        return std::visit([t](const auto& curve) { return curve.derivative(t); }, _curve);
    }

    Vec2 tangent(float t) const {
        return std::visit([t](const auto& curve) { return curve.tangent(t); }, _curve);
    }

    std::pair<Curve2D, Curve2D> split(float t) const {
        return std::visit(
            [t](const auto& curve) {
                auto [first, second] = curve.split(t);
                return std::make_pair(Curve2D(first), Curve2D(second));
            },
            _curve);
    }

    bool isFlat(float tolerance) const {
        return std::visit([tolerance](const auto& curve) { return curve.isFlat(tolerance); }, _curve);
    }

   private:
    std::variant<LineSegment2D, Arc2D, QuadraticBezier2D, CubicBezier2D> _curve;
};

}  // namespace robot::geometry
