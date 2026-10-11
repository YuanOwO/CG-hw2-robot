#pragma once

#include <cstdlib>
#include <vector>

#include "math/constants.hpp"
#include "math/vec2d.hpp"

namespace robot {

// 點座標，必要時也可以用於表示向量（例如兩點之間的向量）。
struct Point {
    Point() : x(0.0f), y(0.0f) {}
    Point(float x, float y) : x(x), y(y) {}

    float x, y;

    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    bool operator!=(const Point& other) const { return !(*this == other); }

    Vec2D operator-(const Point& other) const { return Vec2D(x - other.x, y - other.y); }
};

}  // namespace robot
