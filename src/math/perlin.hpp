#pragma once

#include <cstdint>

#include "math/random.hpp"
#include "math/utils.hpp"
#include "math/vec2.hpp"
#include "math/vec3.hpp"

namespace robot {

namespace detail {

// 與同一 seed 的其他隨機用途分開，雜訊才不會和它們相關
constexpr Stream DEFAULT_PERLIN_1D_STREAM = fnv1a32("PERLIN_1D");
constexpr Stream DEFAULT_PERLIN_2D_STREAM = fnv1a32("PERLIN_2D");
constexpr Stream DEFAULT_PERLIN_3D_STREAM = fnv1a32("PERLIN_3D");

}  // namespace detail

// Perlin 梯度雜訊。輸出只由 seed 與座標決定，與取樣順序無關，
// 因此分開生成的相鄰區塊在共用邊界上會得到相同的值

/// 1D Perlin 梯度雜訊，輸出範圍 [-1, 1]；整數格點上為 0
class Perlin1D {
   public:
    explicit Perlin1D(Seed seed, Stream stream = detail::DEFAULT_PERLIN_1D_STREAM);
    ~Perlin1D() = default;

    float noise(float x) const;

   private:
    Random _random;
    Stream _stream;

    float gradientAt(std::int64_t x) const;
};

/// 2D Perlin 梯度雜訊，輸出範圍約 [-1, 1]；整數格點上為 0
class Perlin2D {
   public:
    explicit Perlin2D(Seed seed, Stream stream = detail::DEFAULT_PERLIN_2D_STREAM);
    ~Perlin2D() = default;

    float noise(float x, float y) const;

   private:
    Random _random;
    Stream _stream;

    Vec2 gradientAt(std::int64_t x, std::int64_t y) const;
};

/// 3D Perlin 梯度雜訊，輸出範圍約 [-1, 1]；整數格點上為 0
class Perlin3D {
   public:
    explicit Perlin3D(Seed seed, Stream stream = detail::DEFAULT_PERLIN_3D_STREAM);
    ~Perlin3D() = default;

    float noise(float x, float y, float z) const;

   private:
    Random _random;
    Stream _stream;

    Vec3 gradientAt(std::int64_t x, std::int64_t y, std::int64_t z) const;
};

}  // namespace robot
