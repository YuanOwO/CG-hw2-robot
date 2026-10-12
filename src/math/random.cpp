#include "math/random.hpp"

#include <cassert>
#include <cmath>

namespace robot {
namespace {

// 領域分隔碼讓相同的 seed、座標、stream 與 index 在 1D、2D 和 3D 取得不同結果。
// 這些值只需固定且彼此不同；修改任一值會改變既有世界的生成結果。
// 取自 SHA-512 的初始雜湊值（前幾個質數平方根的小數部分），
// 來源公開可查，表示這些數字沒有刻意挑選過。
constexpr std::uint64_t DOMAIN_0D = 0x9b05688c2b3e6c1fULL;
constexpr std::uint64_t DOMAIN_1D = 0x6a09e667f3bcc908ULL;
constexpr std::uint64_t DOMAIN_2D = 0xbb67ae8584caa73bULL;
constexpr std::uint64_t DOMAIN_3D = 0x510e527fade682d1ULL;
constexpr std::uint64_t STREAM_TAG = 0x3c6ef372fe94f82bULL;
constexpr std::uint64_t INDEX_TAG = 0xa54ff53a5f1d36f1ULL;

// 重試時用來導出另一個候選值的固定奇數步長。
constexpr std::uint64_t RETRY_STEP = 0x9e3779b97f4a7c15ULL;

}  // namespace

Random::Random(std::uint64_t seed) : _seed(seed) {}

std::uint64_t Random::mix64(std::uint64_t value) {
    // SplitMix64 的 finalizer。無號整數溢位具有 modulo 2^64 的定義，
    // 因此各平台會得到相同的位元結果。
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

std::uint64_t Random::valueAt(std::uint32_t stream, std::uint32_t index) const {
    std::uint64_t value = mix64(_seed ^ DOMAIN_0D);
    value = mix64(value ^ STREAM_TAG ^ stream);
    return mix64(value ^ INDEX_TAG ^ index);
}

std::uint64_t Random::valueAt1D(std::int64_t x, std::uint32_t stream, std::uint32_t index) const {
    // 負座標轉為 uint64_t 時依 modulo 2^64 規則處理，仍可穩定重現。
    std::uint64_t value = mix64(_seed ^ DOMAIN_1D);
    value = mix64(value ^ static_cast<std::uint64_t>(x));
    value = mix64(value ^ STREAM_TAG ^ stream);
    return mix64(value ^ INDEX_TAG ^ index);
}

std::uint64_t Random::valueAt3D(std::int64_t x, std::int64_t y, std::int64_t z, std::uint32_t stream,
                                std::uint32_t index) const {
    std::uint64_t value = mix64(_seed ^ DOMAIN_3D);
    value = mix64(value ^ static_cast<std::uint64_t>(x));
    value = mix64(value ^ static_cast<std::uint64_t>(y));
    value = mix64(value ^ static_cast<std::uint64_t>(z));
    value = mix64(value ^ STREAM_TAG ^ stream);
    return mix64(value ^ INDEX_TAG ^ index);
}

std::uint64_t Random::valueAt2D(std::int64_t x, std::int64_t y, std::uint32_t stream, std::uint32_t index) const {
    std::uint64_t value = mix64(_seed ^ DOMAIN_2D);
    value = mix64(value ^ static_cast<std::uint64_t>(x));
    value = mix64(value ^ static_cast<std::uint64_t>(y));
    value = mix64(value ^ STREAM_TAG ^ stream);
    return mix64(value ^ INDEX_TAG ^ index);
}

std::uint32_t Random::uniformInt(std::uint64_t value, std::uint32_t minimum, std::uint32_t maximum) {
    assert(minimum <= maximum);
    const std::uint64_t range = static_cast<std::uint64_t>(maximum) - minimum + 1u;

    // 直接取 value % range 時，若 2^64 不能被 range 整除，部分結果會較常出現。
    // [threshold, 2^64) 的長度可被 range 整除，因此此區間可安全取餘數。
    const std::uint64_t threshold = (std::uint64_t{0} - range) % range;
    std::uint64_t candidate = value;
    std::uint64_t retry = 0;
    while (candidate < threshold) {
        ++retry;
        // uniformInt 只收到一個值，因此以固定步長導出下一個候選值。
        // 若未來需要嚴格的獨立重試，應由呼叫端以新的 valueAt* index 提供候選值。
        candidate = mix64(value + retry * RETRY_STEP);
    }
    return minimum + static_cast<std::uint32_t>(candidate % range);
}

double Random::uniformFloat(std::uint64_t value, double minimum, double maximum) {
    assert(std::isfinite(minimum));
    assert(std::isfinite(maximum));
    assert(minimum <= maximum);

    if (minimum == maximum) {
        return minimum;
    }

    // double 的尾數有 53 位元；取高位形成 2^-53 間距的 [0, 1) 小數。
    constexpr double UNIT_SCALE = 0x1.0p-53;
    const double unit = static_cast<double>(value >> 11) * UNIT_SCALE;

    const double result = minimum + (maximum - minimum) * unit;

    // 浮點四捨五入可能使接近 maximum 的值碰到上界，退回前一個可表示值。
    return result < maximum ? result : std::nextafter(maximum, minimum);
}

}  // namespace robot
