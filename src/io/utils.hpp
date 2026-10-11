#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace robot::io::utils {

void validateWritePath(const fs::path& path);
void validateReadPath(const fs::path& path);

}  // namespace robot::io::utils
