#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "common/bytes.hpp"

namespace robot::io::serializer {

// 在一段 little-endian 二進位資料上依序讀取欄位，讀取後游標自動前進，不必手算各欄位的位移。
// 呼叫端應事先確認資料長度足夠；讀超過結尾屬於程式錯誤，會拋出 std::out_of_range。
class ByteReader {
   public:
    ByteReader(const std::uint8_t* data, std::size_t size) : _data(data), _size(size) {}
    explicit ByteReader(const std::vector<std::uint8_t>& bytes) : ByteReader(bytes.data(), bytes.size()) {}

    std::uint8_t u8() { return *take(1); }
    std::uint16_t u16() { return bytes::readU16(take(2)); }
    std::int16_t i16() { return bytes::readI16(take(2)); }
    std::uint32_t u32() { return bytes::readU32(take(4)); }

    void read(void* destination, std::size_t size) { std::memcpy(destination, take(size), size); }

    std::size_t position() const { return _position; }

   private:
    const std::uint8_t* take(std::size_t size) {
        if (size > _size - _position) {
            throw std::out_of_range("ByteReader read past end of data");
        }

        const std::uint8_t* current = _data + _position;
        _position += size;
        return current;
    }

    const std::uint8_t* _data;
    std::size_t _size;
    std::size_t _position = 0;
};

}  // namespace robot::io::serializer
