#include "io/font_catalog.hpp"

#include <exception>
#include <stdexcept>
#include <utility>
#include <vector>

#include "font/font_library.hpp"
#include "io/font_storage.hpp"
#include "io/utils.hpp"

namespace robot::io {

namespace {

// 載入函式只捕捉路徑，字型檔在第一次用到時才讀取，啟動時不必先載入數十 MB 的字型。
// 檔案是否存在在這裡就先檢查，設定檔中的路徑打錯時，啟動時就會發現，而不是等到第一次用到該字號。
FontLibrary::Entry toEntry(const FontManifestFace& face, const std::filesystem::path& fontDirectory) {
    FontLibrary::Entry entry;

    for (const auto& [pixelSize, file] : face.gfnt) {
        const auto path = fontDirectory / file;
        utils::validateReadPath(path);
        entry.gfnt.emplace(pixelSize, [path] { return FontStorage::readGfnt(path); });
    }

    if (!face.gotl.empty()) {
        const auto path = fontDirectory / face.gotl;
        utils::validateReadPath(path);
        entry.gotl = [path] { return FontStorage::readGotl(path); };
    }

    return entry;
}

}  // namespace

void registerFonts(const std::filesystem::path& manifestPath) {
    const auto faces = FontStorage::readFontManifest(manifestPath);
    const auto fontDirectory = manifestPath.parent_path();

    // 全部檢查通過才註冊，設定檔有錯時不會只註冊一部分。
    std::vector<std::pair<GlyphFont, FontLibrary::Entry>> entries;
    for (const auto& face : faces) {
        try {
            entries.emplace_back(face.font, toEntry(face, fontDirectory));
        } catch (const std::exception& error) {
            throw std::runtime_error(manifestPath.string() + ": [" + glyphFontName(face.font) + "] " + error.what());
        }
    }

    for (auto& [font, entry] : entries) {
        fonts().add(font, std::move(entry));
    }
}

}  // namespace robot::io
