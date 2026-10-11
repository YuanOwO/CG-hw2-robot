#include "io/serializer/binary_parser.hpp"

#include <cstddef>

#include "io/serializer/utils.hpp"

namespace robot::io::serializer {

BinaryParser::BinaryParser(std::istream& input, const std::string& formatName)
    : _input(input), _formatName(formatName), _start(input.tellg()), _size(0) {
    try {
        _size = utils::remainingSize(_input);
    } catch (const std::runtime_error&) {
        throw error("input must be seekable");
    }
}

std::runtime_error BinaryParser::error(const std::string& message) const {
    return std::runtime_error("Invalid " + _formatName + " file: " + message);
}

std::vector<std::uint8_t> BinaryParser::readBytes(std::uint64_t size) {
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));

    if (!utils::readExact(_input, bytes.data(), bytes.size())) {
        throw error("unexpected end of file");
    }

    return bytes;
}

std::vector<std::uint8_t> BinaryParser::readSection(std::uint64_t offset, std::uint64_t size) {
    _input.clear();
    _input.seekg(_start + static_cast<std::streamoff>(offset));

    if (!_input) {
        throw error("failed to seek to offset " + std::to_string(offset));
    }

    return readBytes(size);
}

}  // namespace robot::io::serializer
