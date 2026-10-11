#pragma once

#include <istream>

#include "font/gfnt_font.hpp"
#include "io/serializer/binary_parser.hpp"

namespace robot::io {

class GfntParserV1 : public serializer::BinaryParser {
   public:
    // input 需以 binary mode 開啟且可 seek，用來確認檔案大小。
    explicit GfntParserV1(std::istream& input) : BinaryParser(input, "GFNT v1") {}

    // 讀取並驗證整個 GFNT v1 字型；格式錯誤或資料截斷時拋出例外。
    GfntFont parse();
};

}  // namespace robot::io
