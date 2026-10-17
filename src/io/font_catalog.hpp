#pragma once

#include <filesystem>

namespace robot::io {

// 依字型設定檔把每個字型加入 fonts()。設定檔中的字型路徑相對於設定檔所在的資料夾；
// 字型檔在第一次用到時才讀取。
void registerFonts(const std::filesystem::path& manifestPath);

}  // namespace robot::io
