#include "io/serializer/core/ini_parser.hpp"

#include <algorithm>
#include <string>

#include "io/serializer/core/utils.hpp"

namespace robot::io::serializer {

namespace {

const std::string UTF8_BOM = "\xEF\xBB\xBF";

// 行尾的 \r 已由 readLine 移除，這裡只需處理空白與 tab。
std::string trim(const std::string& text) {
    const auto isSpace = [](char c) { return c == ' ' || c == '\t'; };

    const auto begin = std::find_if_not(text.begin(), text.end(), isSpace);
    const auto end = std::find_if_not(text.rbegin(), std::string::const_reverse_iterator(begin), isSpace).base();
    return std::string(begin, end);
}

}  // namespace

IniParser::IniParser(std::istream& input, const std::string& formatName) : _input(input), _formatName(formatName) {}

#pragma region Errors

std::runtime_error IniParser::error(const std::string& message, std::size_t lineNumber) const {
    return std::runtime_error("Invalid " + _formatName + " file at line " + std::to_string(lineNumber) + ": " +
                              message);
}

std::runtime_error IniParser::error(const std::string& message) const {
    return std::runtime_error("Invalid " + _formatName + " file: " + message);
}

#pragma endregion  // Errors

#pragma region Parsing

std::vector<IniSection> IniParser::parseSections() {
    std::vector<IniSection> sections;

    std::string rawLine;
    std::size_t lineNumber = 0;

    while (utils::readLine(_input, rawLine)) {
        lineNumber++;

        // 部分 Windows 編輯器會在檔案開頭加上 UTF-8 BOM。
        if (lineNumber == 1 && rawLine.compare(0, UTF8_BOM.size(), UTF8_BOM) == 0) {
            rawLine.erase(0, UTF8_BOM.size());
        }

        if (!utils::isValidUtf8(rawLine)) {
            throw error("line is not valid UTF-8", lineNumber);
        }

        const std::string line = trim(rawLine);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line[0] == '[') {
            if (line.back() != ']') {
                throw error("section header must end with ']'", lineNumber);
            }

            const std::string name = trim(line.substr(1, line.size() - 2));
            if (name.empty()) {
                throw error("section name must not be empty", lineNumber);
            }

            const auto duplicated = std::find_if(sections.begin(), sections.end(),
                                                 [&name](const IniSection& section) { return section.name == name; });
            if (duplicated != sections.end()) {
                throw error(
                    "duplicate section [" + name + "], first defined at line " + std::to_string(duplicated->lineNumber),
                    lineNumber);
            }

            sections.push_back(IniSection{name, lineNumber, {}});
            continue;
        }

        // key = value；value 中可以再出現 '='，只以第一個 '=' 分隔。
        const std::size_t equals = line.find('=');
        if (equals == std::string::npos) {
            throw error("expected [section] or key = value", lineNumber);
        }

        const std::string key = trim(line.substr(0, equals));
        if (key.empty()) {
            throw error("key must not be empty", lineNumber);
        }

        if (sections.empty()) {
            throw error("key '" + key + "' appears before any section", lineNumber);
        }

        auto& entries = sections.back().entries;
        const auto duplicated =
            std::find_if(entries.begin(), entries.end(), [&key](const IniEntry& entry) { return entry.key == key; });
        if (duplicated != entries.end()) {
            throw error("duplicate key '" + key + "', first defined at line " + std::to_string(duplicated->lineNumber),
                        lineNumber);
        }

        entries.push_back(IniEntry{key, trim(line.substr(equals + 1)), lineNumber});
    }

    return sections;
}

#pragma endregion  // Parsing

}  // namespace robot::io::serializer
