#pragma once

#include <algorithm>
#include <vector>

namespace robot::glyph_lookup {

// GFNT 與 GOTL 的 glyph table 都依 codepoint 嚴格遞增排列（載入時已驗證），可以二分搜尋。
template <typename Glyph>
const Glyph* find(const std::vector<Glyph>& glyphs, char32_t codepoint) {
    const auto it = std::lower_bound(glyphs.begin(), glyphs.end(), codepoint,
                                     [](const Glyph& glyph, char32_t searched) { return glyph.codepoint < searched; });

    if (it == glyphs.end() || it->codepoint != codepoint) {
        return nullptr;
    }
    return &*it;
}

// 缺字時依序以 U+FFFD、'?' 替代；都沒有時回傳 nullptr。
template <typename Glyph>
const Glyph* findOrFallback(const std::vector<Glyph>& glyphs, char32_t codepoint) {
    for (const char32_t candidate : {codepoint, U'�', U'?'}) {
        if (const Glyph* glyph = find(glyphs, candidate)) {
            return glyph;
        }
    }
    return nullptr;
}

}  // namespace robot::glyph_lookup
