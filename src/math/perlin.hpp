#pragma once

#include <cstdint>

#include "math/random.hpp"
#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot {

// Perlin 梯度雜訊。梯度只由 seed 與整數格點座標決定，與取樣順序無關，
// 因此分開生成的相鄰區塊在共用邊界上會得到相同的值。

// 輸出範圍 [-0.5, 0.5]。
class Perlin1D {
   public:
    explicit Perlin1D(std::uint64_t seed);
    ~Perlin1D() = default;

    float noise(float x) const;

   private:
    Random _random;

    float gradientAt(std::int64_t x) const;
};

// 梯度為單位向量，輸出範圍約 [-0.71, 0.71]。
class Perlin2D {
   public:
    explicit Perlin2D(std::uint64_t seed);
    ~Perlin2D() = default;

    float noise(float x, float y) const;

   private:
    Random _random;

    Vec2 gradientAt(std::int64_t x, std::int64_t y) const;
};

// 梯度長度為 √2 而未正規化，輸出範圍約 [-1, 1]。
class Perlin3D {
   public:
    explicit Perlin3D(std::uint64_t seed);
    ~Perlin3D() = default;

    float noise(float x, float y, float z) const;

   private:
    Random _random;

    Vec3 gradientAt(std::int64_t x, std::int64_t y, std::int64_t z) const;
};

}  // namespace robot
