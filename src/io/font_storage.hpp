#pragma once

#include <filesystem>

#include "font/gfnt_font.hpp"
#include "font/gotl_font.hpp"

namespace robot::io {

// 讀取字型相關檔案。整份檔案驗證通過才回傳，失敗時拋出帶有檔名的例外。
class FontStorage {
   public:
    static GfntFont readGfnt(const std::filesystem::path& filename);
    static GotlFont readGotl(const std::filesystem::path& filename);
};

}  // namespace robot::io
