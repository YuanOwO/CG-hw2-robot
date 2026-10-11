#include "io/font_storage.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

#include "io/serializer/gfnt_parser_v1.hpp"
#include "io/utils.hpp"

namespace robot::io {

GfntFont FontStorage::readGfnt(const std::filesystem::path& filename) {
    utils::validateReadPath(filename);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + filename.string());
    }

    // 目前只有 V1；新增格式版本時，可以在這一層依檔頭選擇對應的 parser。
    GfntParserV1 parser(file);
    return parser.parse();
}

}  // namespace robot::io
