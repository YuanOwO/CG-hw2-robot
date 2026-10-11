#pragma once

#include <cstddef>
#include <istream>
#include <string>
#include <vector>

#include "io/font_manifest.hpp"
#include "io/serializer/core/ini_parser.hpp"

namespace robot::io {

// 字型設定檔（INI）。每個 section 對應一個 GlyphFont，名稱見 GLYPH_FONT_NAMES：
//
//   [NOTO_SANS_CJK_TC]
//   gfnt.12 = NotoSansCJKtc/NotoSansCJKtc-Regular-12.gfnt （key 中的數字為字號，可有多筆）
//   gotl = NotoSansCJKtc/NotoSansCJKtc-Regular.gotl       （可省略）
//
// 每個 GlyphFont 都必須恰好出現一次，且至少有一個字型檔。
class FontManifestParser : public serializer::IniParser {
   public:
    explicit FontManifestParser(std::istream& input) : IniParser(input, "font manifest") {}

    // 整份設定檔合法才回傳；錯誤時拋出例外並標出行號。
    std::vector<FontManifestFace> parse();

   private:
    GlyphFont parseFont(const serializer::IniSection& section) const;
    int parsePixelSize(const std::string& text, std::size_t lineNumber) const;
    std::string parsePath(const serializer::IniEntry& entry) const;

    FontManifestFace parseFace(const serializer::IniSection& section) const;
};

}  // namespace robot::io
