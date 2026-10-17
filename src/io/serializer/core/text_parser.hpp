#pragma once

#include <cstddef>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace robot::io::serializer {

// 一筆以空白分隔的文字記錄。保留行號是為了讓各格式能提供清楚的錯誤訊息。
struct Record {
    std::vector<std::string> fields;
    std::size_t lineNumber;
};

// 文字格式 parser 的父類別，只處理輸入、型別與行號，不理解任何格式的 grammar。
class TextParser {
   protected:
    TextParser(std::istream& input, const std::string& formatName);

    // 錯誤訊息標出格式名稱與行號；lineNumber 為 0 時指向下一筆應該出現的記錄。
    std::runtime_error error(const std::string& message, std::size_t lineNumber = 0) const;

    // 格式中不允許空行，遇到空行時拋出例外。
    Record readRecord();
    Record expectRecord(const char* keyword, std::size_t fieldCount);

    // field 是欄位的說明文字，只用於錯誤訊息。
    float parseFloat(const std::string& token, const char* field, std::size_t lineNumber) const;
    int parseNonNegativeInt(const std::string& token, const char* field, std::size_t lineNumber) const;
    std::size_t parseCount(const std::string& token, const char* field, std::size_t lineNumber) const;

    // 讀取有明確 byte 長度的 UTF-8 內容，並吃掉內容後方的換行分隔符。
    std::string readUtf8Payload(std::size_t byteCount);

    // 允許檔尾有空白，但不允許再出現其他資料。
    void expectEnd();

   private:
    std::istream& _input;
    std::string _formatName;
    std::size_t _lineNumber = 0;  // 最後讀完的一行
};

}  // namespace robot::io::serializer
