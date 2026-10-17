#pragma once

#include <cstdint>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace robot::io::serializer {

// 二進位格式 parser 的父類別，只處理 stream、區段讀取與錯誤訊息，不理解任何格式的結構。
// 讀進來的 bytes 交給 ByteReader 逐欄解析。
class BinaryParser {
   protected:
    // input 需以 binary mode 開啟且可 seek；目前位置視為檔案起點。
    BinaryParser(std::istream& input, const std::string& formatName);

    std::runtime_error error(const std::string& message) const;

    // 從檔案起點到結尾的 byte 數，用來在配置記憶體前檢查 header 中的大小欄位。
    std::uint64_t inputSize() const { return _size; }

    // 資料不足時拋出例外，不會回傳只讀到一部分的資料。
    std::vector<std::uint8_t> readBytes(std::uint64_t size);

    // 從相對於檔案起點的 offset 讀取，用於 header 指定位置的區段。
    std::vector<std::uint8_t> readSection(std::uint64_t offset, std::uint64_t size);

   private:
    std::istream& _input;
    std::string _formatName;
    std::streampos _start;  // 建構時的讀取位置，readSection 的 offset 以此為基準
    std::uint64_t _size;    // 從 _start 到結尾的 byte 數
};

}  // namespace robot::io::serializer
