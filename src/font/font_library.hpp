#pragma once

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

#include "font/data/gfnt_data.hpp"
#include "font/data/gotl_data.hpp"
#include "font/font.hpp"
#include "font/sized_font.hpp"

namespace robot {

// 管理所有 GlyphFont：依字號決定字形來源，並建立、保存各字號的 SizedFont。
//
// 一個字型可以同時有多個字號的 GFNT 與一份 GOTL，依序選擇：
//   1. 有該字號的 GFNT：直接使用。GFNT 保有 hinting，小字號的品質比點陣化 GOTL 好，
//      因此哪些字號用 GFNT 由提供了哪些字號的 GFNT 決定。
//   2. 有 GOTL：點陣化成該字號。
//   3. 字號是某個 GFNT 字號的整數倍：整數倍放大，像素字型放大後仍是銳利的方塊。
//
// 字型檔在第一次用到時才讀取；FontLibrary 不知道檔案從哪裡來，只呼叫 Entry 中的載入函式。
class FontLibrary {
   public:
    struct Entry {
        std::map<int, std::function<GfntData()>> gfnt;  // 字號 → 載入函式；字號不會重複
        std::function<GotlData()> gotl;                 // 空的函式表示沒有 GOTL
    };

    // 只在啟動時呼叫。同一個字型重複加入時拋出例外，避免已取得的 SizedFont 被換掉。
    void add(GlyphFont font, Entry entry);

    // 回傳的參考在 FontLibrary 存在期間都有效。
    const SizedFont& get(GlyphFont font, int pixelSize);
    const SizedFont& get(const GlyphFontStyle& style) { return get(style.font, style.pixelSize); }

   private:
    struct GfntSlot {
        std::function<GfntData()> load;
        std::unique_ptr<GfntData> data;  // 尚未載入時為 nullptr
    };

    struct Family {
        // key 為字號。用有序的 map，整數倍放大時才能由大到小找出可整除的字號。
        std::map<int, GfntSlot> gfnt;
        std::function<GotlData()> loadGotl;
        std::unique_ptr<GotlData> gotl;  // 尚未載入時為 nullptr

        // key 為字號。只需要精確查詢，不需要順序；SizedFont 以 unique_ptr 保存，位址不會因新增而改變。
        std::unordered_map<int, std::unique_ptr<SizedFont>> fonts;
    };

    std::unique_ptr<SizedFont> create(Family& family, int pixelSize);

    // slot 不保存字號（字號是 map 的 key），由呼叫端傳入，用來檢查檔案內容。
    static const GfntData& loadGfnt(int pixelSize, GfntSlot& slot);
    static const GotlData& loadGotl(Family& family);

    std::unordered_map<GlyphFont, Family> _families;
};

// 全域的字型庫，由啟動流程依字型設定檔填入。
FontLibrary& fonts();

}  // namespace robot
