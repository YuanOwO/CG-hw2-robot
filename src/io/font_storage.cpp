#include "io/font_storage.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>

#include "io/serializer/gfnt_parser_v1.hpp"
#include "io/utils.hpp"

namespace robot::io {

namespace {

// 以 binary mode 開檔並解析；解析錯誤會補上檔名，方便找出是哪個字型檔有問題。
template <typename Parser>
auto parseFile(const std::filesystem::path& filename) {
    utils::validateReadPath(filename);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + filename.string());
    }

    try {
        Parser parser(file);
        return parser.parse();
    } catch (const std::exception& error) {
        throw std::runtime_error(filename.string() + ": " + error.what());
    }
}

}  // namespace

GfntFont FontStorage::readGfnt(const std::filesystem::path& filename) {
    // 目前只有 V1；新增格式版本時，可以在這一層依檔頭選擇對應的 parser。
    return parseFile<GfntParserV1>(filename);
}

}  // namespace robot::io
