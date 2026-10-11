#pragma once

#include <istream>

#include "font/gfnt_font.hpp"

namespace robot::io {

class GfntParserV1 {
   public:
    // input 需以 binary mode 開啟且可 seek，用來確認檔案大小。
    GfntParserV1(std::istream& input) : _input(input) {}

    // 讀取並驗證整個 GFNT v1 字型；格式錯誤或資料截斷時拋出例外。
    GfntFont parse();

   private:
    std::istream& _input;
};

}  // namespace robot::io
