#pragma once

#include <cstddef>
#include <string>

namespace robot::utf8 {

struct DecodeResult {
    char32_t codepoint;
    std::size_t bytesConsumed;
};

// 單一字元轉換，適合 renderer 和游標操作。
DecodeResult decodeOne(const std::string& text, std::size_t offset);

std::string encodeOne(char32_t codepoint);

// 是否為合法的 Unicode scalar value：≤ U+10FFFF，且不在 surrogate 範圍 U+D800–U+DFFF。
bool isScalarValue(char32_t codepoint);

// 格式化成 "U+4F60" 的形式，供錯誤訊息使用。
std::string formatCodepoint(char32_t codepoint);

// 整個字串轉換，適合一般使用。
std::u32string toUtf32(const std::string& text);
std::string fromUtf32(const std::u32string& text);

}  // namespace robot::utf8
