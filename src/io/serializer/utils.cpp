#include "io/serializer/utils.hpp"

#include <cmath>
#include <locale>
#include <sstream>
#include <stdexcept>

#include "common/utf8.hpp"

namespace robot::io::serializer::utils {

bool readLine(std::istream& input, std::string& line) {
    if (!std::getline(input, line)) {
        return false;
    }

    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return true;
}

std::vector<std::string> splitFields(const std::string& line) {
    std::istringstream stream(line);
    stream.imbue(std::locale::classic());
    std::vector<std::string> fields;
    std::string field;

    while (stream >> field) {
        fields.push_back(field);
    }
    return fields;
}

bool parseFloat(const std::string& text, float& value) {
    std::istringstream stream(text);
    stream.imbue(std::locale::classic());

    char extra;
    return (stream >> value) && !(stream >> extra) && std::isfinite(value);
}

bool parseInteger(const std::string& text, long long& value) {
    std::istringstream stream(text);
    stream.imbue(std::locale::classic());
    char extra;
    return (stream >> value) && !(stream >> extra);
}

bool readExact(std::istream& input, std::string& data, std::size_t size) {
    data.assign(size, '\0');
    return readExact(input, data.data(), size);
}

bool readExact(std::istream& input, void* destination, std::size_t size) {
    if (size == 0) {
        return true;
    }

    input.read(static_cast<char*>(destination), static_cast<std::streamsize>(size));
    return input.gcount() == static_cast<std::streamsize>(size);
}

std::uint64_t remainingSize(std::istream& input) {
    const std::streampos start = input.tellg();
    input.seekg(0, std::ios::end);
    const std::streampos end = input.tellg();
    input.seekg(start);

    if (start < 0 || end < 0 || !input) {
        throw std::runtime_error("Input must be seekable");
    }

    return static_cast<std::uint64_t>(end - start);
}

bool isValidUtf8(const std::string& text) {
    std::size_t offset = 0;

    while (offset < text.size()) {
        const auto decoded = utf8::decodeOne(text, offset);
        const std::string encoded = utf8::encodeOne(decoded.codepoint);

        // decodeOne 會將錯誤輸入改成 U+FFFD。再編碼並比較原文，
        // 才能區分無效輸入與文字中真正的 U+FFFD。
        if (decoded.bytesConsumed == 0 || encoded.size() != decoded.bytesConsumed ||
            text.compare(offset, decoded.bytesConsumed, encoded) != 0) {
            return false;
        }

        offset += decoded.bytesConsumed;
    }
    return true;
}

}  // namespace robot::io::serializer::utils
