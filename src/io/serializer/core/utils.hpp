#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

namespace robot::io::serializer::utils {

// 各 parser 共用的低階工具。除了 remainingSize，失敗時都只回傳 false，錯誤訊息由呼叫端的 parser 組成。

#pragma region Text lines

// 讀取一行並統一移除 LF 或 CRLF。
bool readLine(std::istream& input, std::string& line);

// 將記錄依空白分成欄位，不在這一層解釋任何格式關鍵字。
std::vector<std::string> splitFields(const std::string& line);

#pragma endregion  // Text lines

#pragma region Number parsing

// 固定使用 C locale；只接受完整數字，尾端含有其他字元時回傳 false。
bool parseFloat(const std::string& text, float& value);  // 另外拒絕 inf 與 NaN
bool parseInteger(const std::string& text, long long& value);

#pragma endregion  // Number parsing

#pragma region Raw input

// 按指定 byte 數讀取，適合文字 payload 或二進位資料；資料不足時回傳 false。
bool readExact(std::istream& input, std::string& data, std::size_t size);
bool readExact(std::istream& input, void* destination, std::size_t size);

// 從目前位置到結尾的 byte 數，不移動讀取位置；input 無法 seek 時拋出例外。
std::uint64_t remainingSize(std::istream& input);

#pragma endregion  // Raw input

#pragma region Validation

// 檢查是否為合法的 UTF-8：拒絕截斷的序列、overlong encoding、surrogate 與超出 U+10FFFF 的值。
bool isValidUtf8(const std::string& text);

#pragma endregion  // Validation

}  // namespace robot::io::serializer::utils
