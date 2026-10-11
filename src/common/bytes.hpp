#pragma once

#include <cstdint>

namespace robot::bytes {

// 從 bytes 讀取 little-endian 整數。逐 byte 組合，不依賴執行平台的 endian，也不要求指標對齊。

inline std::uint16_t readU16(const std::uint8_t* bytes) {
    return static_cast<std::uint16_t>(bytes[0]) | (static_cast<std::uint16_t>(bytes[1]) << 8);
}

inline std::int16_t readI16(const std::uint8_t* bytes) {
    const std::uint16_t value = readU16(bytes);

    if (value <= 0x7FFF) {
        return static_cast<std::int16_t>(value);
    }

    // 用明確的運算還原 two's-complement，不依賴 unsigned 轉 signed
    // 超出範圍時的 implementation-defined 行為。
    return static_cast<std::int16_t>(static_cast<std::int32_t>(value) - 0x10000);
}

inline std::uint32_t readU32(const std::uint8_t* bytes) {
    return static_cast<std::uint32_t>(bytes[0]) | (static_cast<std::uint32_t>(bytes[1]) << 8) |
           (static_cast<std::uint32_t>(bytes[2]) << 16) | (static_cast<std::uint32_t>(bytes[3]) << 24);
}

}  // namespace robot::bytes
