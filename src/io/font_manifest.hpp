#pragma once

#include <map>
#include <string>

#include "font/font.hpp"

namespace robot::io {

// 字型設定檔中的一個字型。路徑相對於設定檔所在的資料夾。
struct FontManifestFace {
    GlyphFont font;

    std::map<int, std::string> gfnt;  // 字號 → 檔案；以字號為 key，同一字號只會有一個檔案
    std::string gotl;                 // 空字串表示沒有 GOTL
};

}  // namespace robot::io
