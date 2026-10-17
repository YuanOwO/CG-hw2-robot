#include "io/serializer/core/binary_parser.hpp"

#include <cstddef>

#include "io/serializer/core/utils.hpp"

namespace robot::io::serializer {

BinaryParser::BinaryParser(std::istream& input, const std::string& formatName)
    : _input(input), _formatName(formatName), _start(input.tellg()), _size(0) {
    // 統一成帶有格式名稱的錯誤訊息。
    try {
        _size = utils::remainingSize(_input);
    } catch (const std::runtime_error&) {
        throw error("input must be seekable");
    }
}

#pragma region Errors

std::runtime_error BinaryParser::error(const std::string& message) const {
    return std::runtime_error("Invalid " + _formatName + " file: " + message);
}

#pragma endregion  // Errors

#pragma region Reading

std::vector<std::uint8_t> BinaryParser::readBytes(std::uint64_t size) {
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));

    if (!utils::readExact(_input, bytes.data(), bytes.size())) {
        throw error("unexpected end of file");
    }

    return bytes;
}

std::vector<std::uint8_t> BinaryParser::readSection(std::uint64_t offset, std::uint64_t size) {
    // 前一次讀取可能已到達檔尾並設定 eofbit，seek 前先清除。
    _input.clear();
    _input.seekg(_start + static_cast<std::streamoff>(offset));

    if (!_input) {
        throw error("failed to seek to offset " + std::to_string(offset));
    }

    return readBytes(size);
}

#pragma endregion  // Reading

}  // namespace robot::io::serializer
