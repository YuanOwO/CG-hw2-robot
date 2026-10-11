#include "io/font_storage.hpp"

#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>

#include "io/serializer/font_manifest_parser.hpp"
#include "io/serializer/gfnt_parser_v1.hpp"
#include "io/serializer/gotl_parser_v1.hpp"
#include "io/utils.hpp"

namespace robot::io {

namespace {

// 以 binary mode 開檔並解析；錯誤訊息補上檔名，才知道是哪個檔案有問題。
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

GfntData FontStorage::readGfnt(const std::filesystem::path& filename) {
    return parseFile<GfntParserV1>(filename);
}

GotlData FontStorage::readGotl(const std::filesystem::path& filename) {
    return parseFile<GotlParserV1>(filename);
}

std::vector<FontManifestFace> FontStorage::readFontManifest(const std::filesystem::path& filename) {
    return parseFile<FontManifestParser>(filename);
}

}  // namespace robot::io
