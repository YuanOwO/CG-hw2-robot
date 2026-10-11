#pragma once

#include <istream>

#include "font/data/gfnt_data.hpp"
#include "io/serializer/core/binary_parser.hpp"

namespace robot::io {

class GfntParserV1 : public serializer::BinaryParser {
   public:
    explicit GfntParserV1(std::istream& input) : BinaryParser(input, "GFNT v1") {}

    // 讀取並驗證整個字型，任何一項不合法就拋出例外。
    GfntData parse();
};

}  // namespace robot::io
