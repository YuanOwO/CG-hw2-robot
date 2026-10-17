#pragma once

#include <cstddef>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace robot::io::serializer {

// 一筆 key = value，保留行號供錯誤訊息使用。
struct IniEntry {
    std::string key;
    std::string value;
    std::size_t lineNumber;
};

// 一個 [section] 與其中的 key / value。
struct IniSection {
    std::string name;
    std::size_t lineNumber;
    std::vector<IniEntry> entries;  // 依檔案中出現的順序
};

// INI 格式 parser 的父類別，只把檔案拆成 section 與 key / value，不理解設定的內容。
//
// 支援的語法：
//   [section]       section 名稱前後的空白會去掉
//   key = value     key 與 value 前後的空白會去掉，value 可以含空白
//   # 註解          只支援整行註解；空白行會被忽略；行尾可以是 LF 或 CRLF
// 不支援引號、跳脫字元、行尾註解與多行值。
// 重複的 section、同一個 section 中重複的 key、出現在任何 section 之前的 key 都視為錯誤。
class IniParser {
   protected:
    IniParser(std::istream& input, const std::string& formatName);

    // 沒有行號的版本用於與特定行無關的錯誤，例如缺少某個 section。
    std::runtime_error error(const std::string& message, std::size_t lineNumber) const;
    std::runtime_error error(const std::string& message) const;

    // 格式錯誤時拋出例外並標出行號。
    std::vector<IniSection> parseSections();

   private:
    std::istream& _input;
    std::string _formatName;
};

}  // namespace robot::io::serializer
