#include "font/font_library.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#include "font/gfnt_font.hpp"
#include "font/gotl_font.hpp"

namespace robot {

void FontLibrary::add(GlyphFont font, Entry entry) {
    // Entry 以字號為 key，字號不會重複，這裡只需搬進 slot；檔案留到第一次用到時才載入。
    Family family;
    for (auto& [pixelSize, load] : entry.gfnt) {
        family.gfnt.emplace(pixelSize, GfntSlot{std::move(load), nullptr});
    }
    family.loadGotl = std::move(entry.gotl);

    if (!_families.emplace(font, std::move(family)).second) {
        throw std::logic_error(std::string("Glyph font already added: ") + glyphFontName(font));
    }
}

const SizedFont& FontLibrary::get(GlyphFont font, int pixelSize) {
    if (pixelSize <= 0) {
        throw std::invalid_argument("Font pixel size must be positive: " + std::to_string(pixelSize));
    }

    const auto familyIt = _families.find(font);
    if (familyIt == _families.end()) {
        throw std::logic_error(std::string("Glyph font has not been added: ") + glyphFontName(font));
    }

    Family& family = familyIt->second;

    // 每個字號只建立一次，之後都回傳同一個物件，呼叫端拿到的參考因此一直有效。
    auto it = family.fonts.find(pixelSize);
    if (it == family.fonts.end()) {
        it = family.fonts.emplace(pixelSize, create(family, pixelSize)).first;
    }
    return *it->second;
}

// 選擇順序與理由見 font_library.hpp 的類別說明。
std::unique_ptr<SizedFont> FontLibrary::create(Family& family, int pixelSize) {
    if (const auto it = family.gfnt.find(pixelSize); it != family.gfnt.end()) {
        return std::make_unique<GfntFont>(loadGfnt(pixelSize, it->second), 1);
    }

    // 有輪廓的字型放大後應該是平滑的筆畫，因此 GOTL 優先於整數倍放大。
    if (family.loadGotl) {
        return std::make_unique<GotlFont>(loadGotl(family), pixelSize);
    }

    // 只有 GFNT 的像素字型：用能整除的最大字號放大，倍率最小，筆畫最細。
    // map 依字號遞增排列，由大到小走訪時第一個能整除的就是最大的。
    for (auto it = family.gfnt.rbegin(); it != family.gfnt.rend(); ++it) {
        if (pixelSize % it->first == 0) {
            return std::make_unique<GfntFont>(loadGfnt(it->first, it->second), pixelSize / it->first);
        }
    }

    throw std::invalid_argument("Font has no source for pixel size " + std::to_string(pixelSize));
}

const GfntData& FontLibrary::loadGfnt(int pixelSize, GfntSlot& slot) {
    if (!slot.data) {
        auto data = std::make_unique<GfntData>(slot.load());

        // 選字號時依設定檔的字號（map 的 key）判斷，檔案內容必須相符，否則排版會用錯大小。
        if (data->metrics().pixelSize != pixelSize) {
            throw std::runtime_error("GFNT pixel size mismatch: expected " + std::to_string(pixelSize) +
                                     ", got " + std::to_string(data->metrics().pixelSize));
        }

        slot.data = std::move(data);
    }
    return *slot.data;
}

const GotlData& FontLibrary::loadGotl(Family& family) {
    if (!family.gotl) {
        family.gotl = std::make_unique<GotlData>(family.loadGotl());
    }
    return *family.gotl;
}

// 宣告在 font.hpp，讓上層只需 include 主入口；量測需要查 fonts()，因此實作放在這裡。
float getFontWidth(const GlyphFontStyle& style, const std::string& text) {
    return fonts().get(style).measureWidth(text);
}

float getFontHeight(const GlyphFontStyle& style, const std::string& text) {
    return fonts().get(style).measureHeight(text);
}

FontLibrary& fonts() {
    static FontLibrary library;
    return library;
}

}  // namespace robot
