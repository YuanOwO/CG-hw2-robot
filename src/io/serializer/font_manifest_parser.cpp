#include "io/serializer/font_manifest_parser.hpp"

#include <algorithm>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#include "io/serializer/core/utils.hpp"

namespace robot::io {

namespace {

const std::string GFNT_KEY_PREFIX = "gfnt.";

}  // namespace

#pragma region Values

GlyphFont FontManifestParser::parseFont(const serializer::IniSection& section) const {
    if (const auto font = findGlyphFont(section.name)) {
        return *font;
    }
    throw error("unknown font [" + section.name + "]", section.lineNumber);
}

int FontManifestParser::parsePixelSize(const std::string& text, std::size_t lineNumber) const {
    long long value;
    if (!serializer::utils::parseInteger(text, value) || value <= 0 || value > std::numeric_limits<int>::max()) {
        throw error("pixel size must be a positive integer, got '" + text + "'", lineNumber);
    }
    return static_cast<int>(value);
}

std::string FontManifestParser::parsePath(const serializer::IniEntry& entry) const {
    if (entry.value.empty()) {
        throw error("'" + entry.key + "' must not be empty", entry.lineNumber);
    }

    // 字型檔一律放在設定檔旁邊，避免設定檔指向資料夾以外的檔案。
    const std::filesystem::path path(entry.value);
    if (path.is_absolute() || std::find(path.begin(), path.end(), "..") != path.end()) {
        throw error("font path must be relative and stay inside the font directory: " + entry.value, entry.lineNumber);
    }
    return entry.value;
}

#pragma endregion  // Values

#pragma region Faces

FontManifestFace FontManifestParser::parseFace(const serializer::IniSection& section) const {
    FontManifestFace face{parseFont(section), {}, {}};

    for (const auto& entry : section.entries) {
        if (entry.key.compare(0, GFNT_KEY_PREFIX.size(), GFNT_KEY_PREFIX) == 0) {
            const int pixelSize = parsePixelSize(entry.key.substr(GFNT_KEY_PREFIX.size()), entry.lineNumber);

            // gfnt.12 與 gfnt.012 是不同的 key，INI 層無法發現字號重複；
            // 改以換算後的字號作為 map 的 key，插入失敗即代表重複。
            if (!face.gfnt.emplace(pixelSize, parsePath(entry)).second) {
                throw error("duplicate GFNT pixel size " + std::to_string(pixelSize), entry.lineNumber);
            }
        } else if (entry.key == "gotl") {
            face.gotl = parsePath(entry);
        } else {
            throw error("unknown key '" + entry.key + "'", entry.lineNumber);
        }
    }

    if (face.gfnt.empty() && face.gotl.empty()) {
        throw error("[" + section.name + "] has no font file", section.lineNumber);
    }

    return face;
}

std::vector<FontManifestFace> FontManifestParser::parse() {
    const std::vector<serializer::IniSection> sections = parseSections();

    // 重複的 section 已由 IniParser 拒絕，這裡每個 section 都對應不同的 GlyphFont。
    std::vector<FontManifestFace> faces;
    for (const auto& section : sections) {
        faces.push_back(parseFace(section));
    }

    // 每個 GlyphFont 都要有設定，在啟動時就發現缺少的字型。
    for (const auto& entry : GLYPH_FONT_NAMES) {
        const GlyphFont font = entry.first;
        const bool defined = std::any_of(faces.begin(), faces.end(),
                                         [font](const FontManifestFace& face) { return face.font == font; });
        if (!defined) {
            throw error(std::string("missing section [") + entry.second + "]");
        }
    }

    return faces;
}

#pragma endregion  // Faces

}  // namespace robot::io
