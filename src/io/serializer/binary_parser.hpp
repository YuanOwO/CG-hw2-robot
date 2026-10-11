#pragma once

#include <cstdint>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace robot::io::serializer {

// 共用的二進位格式解析器。它只處理 stream、區段讀取與錯誤訊息，不理解任何特定格式的結構；
// 讀進來的 bytes 再交給 ByteReader 逐欄解析。
class BinaryParser {
   protected:
    // input 需以 binary mode 開啟且可 seek。建構時記下目前位置作為檔案起點，並確認剩餘大小。
    BinaryParser(std::istream& input, const std::string& formatName);

    std::runtime_error error(const std::string& message) const;

    // 從檔案起點到結尾的 byte 數，可用來在配置記憶體前確認 header 中的大小欄位。
    std::uint64_t inputSize() const { return _size; }

    // 從目前位置讀取 size bytes；資料不足時拋出例外，不會把讀到一半的資料當成有效內容。
    std::vector<std::uint8_t> readBytes(std::uint64_t size);

    // 跳到相對於檔案起點的 offset 後讀取 size bytes，適合由 header 指定位置的區段。
    std::vector<std::uint8_t> readSection(std::uint64_t offset, std::uint64_t size);

   private:
    std::istream& _input;
    std::string _formatName;
    std::streampos _start;
    std::uint64_t _size;
};

}  // namespace robot::io::serializer
