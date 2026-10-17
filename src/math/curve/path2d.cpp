#include "math/curve/path2d.hpp"

#include <cassert>
#include <cmath>

#include "math/curve/flatten.hpp"
#include "math/utils.hpp"

namespace robot {
namespace {

// 兩段相接處法向量的夾角在 1° 以內視為平順；切線由 sin、cos 算出，不會剛好相等
const float SMOOTH_JOIN_COSINE = std::cos(radians(1.0f));

Vec2 rightNormal(const Vec2& tangent) {
    return Vec2(tangent.y, -tangent.x);
}

}  // namespace

Path2D::Path2D(Vec2 start) : _start(start), _end(start) {}

Path2D& Path2D::lineTo(const Vec2& end) {
    _curves.emplace_back(LineSegment2D(_end, end));
    _end = end;
    return *this;
}

Path2D& Path2D::arcTo(const Vec2& center, Vec2 end, ArcDirection direction) {
    auto arc = Arc2D::around(_end, center, end, direction);

    assert(arc.has_value());

    if (arc) {
        _curves.emplace_back(*arc);
        _end = arc->endPoint();
    }
    return *this;
}

Path2D& Path2D::tangentArcTo(const Vec2& end) {
    Vec2 direction = _curves.empty() ? Vec2(1.0f, 0.0f) : _curves.back().tangent(1.0f);
    auto arc = Arc2D::tangentAtStart(_end, direction, end);
    if (arc) {
        _curves.emplace_back(*arc);
        _end = end;
    } else {
        // 無法建立圓弧，改用直線
        _curves.emplace_back(LineSegment2D(_end, end));
        _end = end;
    }
    return *this;
}

Path2D& Path2D::quadraticBezierTo(const Vec2& control, const Vec2& end) {
    _curves.emplace_back(QuadraticBezier2D(_end, control, end));
    _end = end;
    return *this;
}

Path2D& Path2D::cubicBezierTo(const Vec2& control1, const Vec2& control2, const Vec2& end) {
    _curves.emplace_back(CubicBezier2D(_end, control1, control2, end));
    _end = end;
    return *this;
}

std::vector<Path2D::Vertex> Path2D::flatten(float tolerance) const {
    std::vector<Vertex> vertices;

    for (const auto& curve : _curves) {
        if (curve.tangent(0.0f) == Vec2(0.0f, 0.0f)) {
            // 曲線起點的切線為零向量，無法決定法線方向，跳過這個曲線
            continue;
        }

        std::vector<CurveSample2D> samples;
        robot::flatten(curve, tolerance, samples);

        for (const auto& sample : samples) {
            Vec2 normal = rightNormal(curve.tangent(sample.t));

            if (!vertices.empty()) {
                auto& prev = vertices.back();
                if (prev.point == sample.point && dot(prev.normal, normal) > SMOOTH_JOIN_COSINE) {
                    // 連續兩個頂點位置相同，且法向量方向相近 => 平順，保留前一個頂點的法向量，略過這個頂點
                    continue;
                }
            }

            vertices.emplace_back(Vertex{sample.point, normal});
        }
    }

    return vertices;
}

}  // namespace robot
