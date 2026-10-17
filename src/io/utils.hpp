#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace robot::io::utils {

// 開檔前先檢查路徑，錯誤訊息才能說明是哪一種問題，而不是只有「無法開啟」。

// 檔案不存在時允許，由寫入的一方新建。
void validateWritePath(const fs::path& path);
void validateReadPath(const fs::path& path);

}  // namespace robot::io::utils
