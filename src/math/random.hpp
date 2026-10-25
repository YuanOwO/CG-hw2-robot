#pragma once

#include <cstdint>

namespace robot {

/// 依 seed、座標、用途與序號取值的無狀態偽隨機來源。
/// 相同輸入永遠回傳相同結果，不受呼叫順序影響
class Random {
   public:
    using Seed = std::uint64_t;
    using Value = std::uint64_t;
    using Stream = std::uint32_t;
    using Index = std::uint32_t;

    explicit Random(Seed seed);
    ~Random() = default;

    Seed seed() const { return _seed; }

    /// 取得一個 64 位元的隨機值；1D、2D、3D 版本再加上整數座標
    /// @param stream 用途，不同用途的值彼此無關
    /// @param index 同一用途、同一座標下的第幾個值
    Value valueAt(Stream stream = 0, Index index = 0) const;
    Value valueAt1D(std::int64_t x, Stream stream = 0, Index index = 0) const;
    Value valueAt2D(std::int64_t x, std::int64_t y, Stream stream = 0, Index index = 0) const;
    Value valueAt3D(std::int64_t x, std::int64_t y, std::int64_t z, Stream stream = 0, Index index = 0) const;

    /// 把 valueAt 的結果換成 [minimum, maximum] 內的整數，分布均勻
    static std::uint32_t uniformInt(Value value, std::uint32_t minimum, std::uint32_t maximum);
    /// 把 valueAt 的結果換成 [minimum, maximum) 內的浮點數
    static double uniformFloat(Value value, double minimum, double maximum);

   private:
    Seed _seed;

    static Value mix64(Value value);
};

using Seed = Random::Seed;
using Stream = Random::Stream;

}  // namespace robot
