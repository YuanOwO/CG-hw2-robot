#pragma once

#include <cstdint>
#include <istream>
#include <vector>

#include "font/gotl_font.hpp"
#include "io/serializer/binary_parser.hpp"
#include "io/serializer/gotl.hpp"

namespace robot::io {

class GotlParserV1 : public serializer::BinaryParser {
   public:
    // input 需以 binary mode 開啟且可 seek，用來確認檔案大小。
    explicit GotlParserV1(std::istream& input) : BinaryParser(input, "GOTL v1") {}

    // 讀取並驗證整個 GOTL v1 字型；不符合規格任何一項合法性條件時拋出例外。
    GotlFont parse();

   private:
    struct BoundingBox;

    // 驗證一個 opCount > 0 的輪廓（規格第 4 節條件 8.1–8.3），回傳所有點的實際 bbox。
    BoundingBox validateOutline(const std::vector<std::uint8_t>& outlineData, const gotl::GotlGlyphV1& glyph,
                                int curveOrder) const;
};

}  // namespace robot::io
