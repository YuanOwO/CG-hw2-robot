#pragma once

#include <cstdint>
#include <istream>
#include <vector>

#include "font/data/gotl_data.hpp"
#include "io/serializer/core/binary_parser.hpp"
#include "io/serializer/format/gotl.hpp"

namespace robot::io {

class GotlParserV1 : public serializer::BinaryParser {
   public:
    explicit GotlParserV1(std::istream& input) : BinaryParser(input, "GOTL v1") {}

    // 讀取並驗證整個字型，任何一項不合法就拋出例外。
    GotlData parse();

   private:
    struct BoundingBox;

    // 回傳輪廓所有點的實際 bbox。共用輪廓的記錄只需呼叫一次，再各自與自己記錄的 bbox 比對。
    BoundingBox validateOutline(const std::vector<std::uint8_t>& outlineData, const gotl::GotlGlyphV1& glyph,
                                int curveOrder) const;
};

}  // namespace robot::io
