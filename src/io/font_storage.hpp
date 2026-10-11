#pragma once

#include <filesystem>

#include "font/gfnt_font.hpp"
#include "font/gotl_font.hpp"

namespace robot::io {

class FontStorage {
   public:
    // 讀取、解析、驗證成功後才回傳；失敗拋出例外。
    static GfntFont readGfnt(const std::filesystem::path& filename);
    static GotlFont readGotl(const std::filesystem::path& filename);
};

}  // namespace robot::io
