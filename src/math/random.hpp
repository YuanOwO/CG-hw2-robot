#pragma once

#include <cstdint>

namespace robot {

// 依 seed、座標、用途與序號取值的無狀態偽隨機來源。
// 相同輸入永遠回傳相同結果，不受呼叫順序影響。
class Random {
   public:
    explicit Random(std::uint64_t seed);
    ~Random() = default;

    std::uint64_t valueAt1D(std::int64_t x, std::uint32_t stream = 0, std::uint32_t index = 0) const;
    std::uint64_t valueAt2D(std::int64_t x, std::int64_t y, std::uint32_t stream = 0, std::uint32_t index = 0) const;
    std::uint64_t valueAt3D(std::int64_t x, std::int64_t y, std::int64_t z, std::uint32_t stream = 0,
                            std::uint32_t index = 0) const;

    // 將 64 位元結果映射至含端點的整數區間或不含上界的浮點區間。
    static std::uint32_t uniformInt(std::uint64_t value, std::uint32_t minimum, std::uint32_t maximum);
    static double uniformFloat(std::uint64_t value, double minimum, double maximum);

   private:
    std::uint64_t _seed;

    static std::uint64_t mix64(std::uint64_t value);
};

}  // namespace robot
