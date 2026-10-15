#include "math/perlin.hpp"

#include <array>
#include <cmath>
#include <cstdint>

#include "math/constants.hpp"
#include "math/utils.hpp"

namespace robot {

// 梯度只由 seed 與整數格點座標決定，相鄰區塊各自取樣時共用邊界的梯度相同

namespace {

// 與同一 seed 的其他隨機用途分開，雜訊才不會和它們相關。
constexpr std::uint32_t PERLIN_1D_STREAM = 0x50455231u;
constexpr std::uint32_t PERLIN_2D_STREAM = 0x50455232u;
constexpr std::uint32_t PERLIN_3D_STREAM = 0x50455233u;

// 表大小取 2 的冪，才能直接取雜湊值的高位元當索引；SplitMix64 的高位元混合得最充分。
constexpr int GRADIENT_BITS = 4;
constexpr int GRADIENT_COUNT = 1 << GRADIENT_BITS;

// 只用 8 個方向時，單一 octave 就看得出 45° 紋路；16 個已幾乎看不出。
// 預先建表，取樣時就不必計算三角函數。單位向量使輸出範圍約為 [-0.71, 0.71]。
const auto GRADIENTS_2D = [] {
    std::array<Vec2, GRADIENT_COUNT> table;
    for (int i = 0; i < GRADIENT_COUNT; ++i) {
        const float angle = 2.0f * PI * i / GRADIENT_COUNT;
        table[i] = {std::cos(angle), std::sin(angle)};
    }
    return table;
}();

// 立方體 12 條邊中點的方向，投影到任何切面上都不整齊，因此看不出方向性。
// 最後 4 個是重複的方向，只為湊滿 16 個；它們出現的機率略高，但肉眼看不出差異。
// 長度為 √2 而未正規化，輸出範圍因此約為 [-1, 1]。
const Vec3 GRADIENTS_3D[GRADIENT_COUNT] = {
    {1,  1,  0 },
    {-1, 1,  0 },
    {1,  -1, 0 },
    {-1, -1, 0 },
    {1,  0,  1 },
    {-1, 0,  1 },
    {1,  0,  -1},
    {-1, 0,  -1},
    {0,  1,  1 },
    {0,  -1, 1 },
    {0,  1,  -1},
    {0,  -1, -1},
    {1,  1,  0 },
    {-1, 1,  0 },
    {0,  -1, 1 },
    {0,  -1, -1},
};

int gradientIndex(std::uint64_t hash) {
    return static_cast<int>(hash >> (64 - GRADIENT_BITS));
}

// f(t) = 6t^5 - 15t^4 + 10t^3。在 t = 0 和 1 的一、二階導數皆為 0，
// 跨格子時曲面平滑相接，不會出現折痕。
float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

}  // namespace

#pragma region Perlin1D

Perlin1D::Perlin1D(std::uint64_t seed) : _random(seed) {}

// 1D 不能像 2D、3D 一樣只用幾個固定梯度：斜率若只有 ±1，每一格的起伏都一樣高，
// 會看出明顯的週期，所以連斜率大小也取隨機值。
float Perlin1D::gradientAt(std::int64_t x) const {
    return static_cast<float>(Random::uniformFloat(_random.valueAt1D(x, PERLIN_1D_STREAM), -1.0, 1.0));
}

float Perlin1D::noise(float x) const {
    const std::int64_t x0 = static_cast<std::int64_t>(std::floor(x));
    const float t = x - x0;

    const float n0 = gradientAt(x0) * t;
    const float n1 = gradientAt(x0 + 1) * (t - 1.0f);

    return lerp(n0, n1, fade(t));
}

#pragma endregion  // Perlin1D

#pragma region Perlin2D

Perlin2D::Perlin2D(std::uint64_t seed) : _random(seed) {}

Vec2 Perlin2D::gradientAt(std::int64_t x, std::int64_t y) const {
    return GRADIENTS_2D[gradientIndex(_random.valueAt2D(x, y, PERLIN_2D_STREAM))];
}

float Perlin2D::noise(float x, float y) const {
    const std::int64_t x0 = static_cast<std::int64_t>(std::floor(x));
    const std::int64_t y0 = static_cast<std::int64_t>(std::floor(y));
    const float tx = x - x0;
    const float ty = y - y0;

    const float u = fade(tx);
    const float v = fade(ty);

    // 由內而外依序沿 y、x 內插：角落 → 邊 → 點。
    float edge[2];
    for (int i = 0; i < 2; i++) {
        float corner[2];
        for (int j = 0; j < 2; j++) {
            corner[j] = dot(gradientAt(x0 + i, y0 + j), Vec2(tx - i, ty - j));
        }
        edge[i] = lerp(corner[0], corner[1], v);
    }

    return lerp(edge[0], edge[1], u);
}

#pragma endregion  // Perlin2D

#pragma region Perlin3D

Perlin3D::Perlin3D(std::uint64_t seed) : _random(seed) {}

Vec3 Perlin3D::gradientAt(std::int64_t x, std::int64_t y, std::int64_t z) const {
    return GRADIENTS_3D[gradientIndex(_random.valueAt3D(x, y, z, PERLIN_3D_STREAM))];
}

float Perlin3D::noise(float x, float y, float z) const {
    const std::int64_t x0 = static_cast<std::int64_t>(std::floor(x));
    const std::int64_t y0 = static_cast<std::int64_t>(std::floor(y));
    const std::int64_t z0 = static_cast<std::int64_t>(std::floor(z));
    const float tx = x - x0;
    const float ty = y - y0;
    const float tz = z - z0;

    const float u = fade(tx);
    const float v = fade(ty);
    const float w = fade(tz);

    // 由內而外依序沿 z、y、x 內插：角落 → 邊 → 面 → 點。
    float face[2];
    for (int i = 0; i < 2; i++) {
        float edge[2];
        for (int j = 0; j < 2; j++) {
            float corner[2];
            for (int k = 0; k < 2; k++) {
                corner[k] = dot(gradientAt(x0 + i, y0 + j, z0 + k), Vec3(tx - i, ty - j, tz - k));
            }
            edge[j] = lerp(corner[0], corner[1], w);
        }
        face[i] = lerp(edge[0], edge[1], v);
    }

    return lerp(face[0], face[1], u);
}

#pragma endregion  // Perlin3D

}  // namespace robot
