#pragma once

#include <filesystem>
#include <vector>

#include "font/data/gfnt_data.hpp"
#include "font/data/gotl_data.hpp"
#include "io/font_manifest.hpp"

namespace robot::io {

// 讀取字型相關檔案。整份檔案驗證通過才回傳，失敗時拋出帶有檔名的例外。
class FontStorage {
   public:
    static GfntData readGfnt(const std::filesystem::path& filename);
    static GotlData readGotl(const std::filesystem::path& filename);

    static std::vector<FontManifestFace> readFontManifest(const std::filesystem::path& filename);
};

}  // namespace robot::io
