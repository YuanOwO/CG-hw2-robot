# 字型

提供文字的量測與排版，支援中文。繪製在 `render/canvas_text.cpp`，內部設計見 [ARCHITECTURE.md](ARCHITECTURE.md)。

## 字型種類

| 種類         | 型別                                   | 字元                | 用途             |
| ------------ | -------------------------------------- | ------------------- | ---------------- |
| GlyphFont    | `GlyphFontStyle{GlyphFont, pixelSize}` | Unicode（UTF-8）    | 一律優先使用     |
| GLUT bitmap  | `BitmapFontStyle{BitmapFont}`          | ASCII               | 相容用途         |
| GLUT stroke  | `StrokeFontStyle{StrokeFont, scale}`   | ASCII               | 相容用途         |

三者合併為 `FontStyle`（`std::variant`），`Canvas::drawText`、`getFontWidth`、`getFontHeight` 都接受 `FontStyle`。

目前的 GlyphFont：

| `GlyphFont`        | 字型                | 可用字號                                     |
| ------------------ | ------------------- | -------------------------------------------- |
| `CUBIC11`          | 俐方體 11 號        | 12 的整數倍（像素字型，放大後仍是銳利方塊）  |
| `UNIFONT`          | GNU Unifont         | 16 的整數倍（像素字型）                      |
| `NOTO_SANS_CJK_TC` | Noto Sans CJK TC    | 任意字號                                     |

## 使用方式

上層只需引入 `font/font.hpp`。樣式只保存「字型 + 字號」，可以寫成全域常數：

```cpp
#include "font/font.hpp"

inline const FontStyle TitleFont = GlyphFontStyle{GlyphFont::NOTO_SANS_CJK_TC, 72};

// position 預設為文字框左上角；anchor 可改為以中心、右下角等位置對齊
canvas.drawText("會動的機器人", TitleFont, color, position);
canvas.drawText("開始", TitleFont, color, center, {render::TextAlign::CENTER, render::TextAlign::CENTER});

// 需要文字框大小時（例如畫背景）再另外量測
const float width = getFontWidth(TitleFont, "會動的機器人");
const float height = getFontHeight(TitleFont, "第一行\n第二行");
```

- 文字以 `'\n'` 換行，每行靠文字框左緣；結尾的換行不會多算一行。
- 文字框的左上角會取整到整數像素，呼叫端不需要自行取整。
- 量出來的大小與畫出來的位置使用同一份排版，兩者一定一致。
- 缺字時依序以 U+FFFD、`?` 替代；都沒有時空出一個字號寬。
- 使用不存在的字號（例如 `CUBIC11` 的 13 px）會拋出例外。

需要逐字的位置或字形時，直接取得 `SizedFont`：

```cpp
#include "font/font_library.hpp"

const SizedFont& font = fonts().get(GlyphFont::UNIFONT, 32);
const TextLayout layout = font.layout("Hello");  // 每個字形的位置與 bitmap
```

## 字型設定檔

啟動時 `engine` 以執行檔旁的 `assets/fonts/fonts.ini` 呼叫 `io::registerFonts`，註冊所有 GlyphFont。每個 section 對應一個 `GlyphFont`，路徑相對於設定檔所在的資料夾：

```ini
[NOTO_SANS_CJK_TC]
gfnt.12 = NotoSansCJKtc/NotoSansCJKtc-Regular-12.gfnt   # 事先點陣化的字號，可有多筆
gotl = NotoSansCJKtc/NotoSansCJKtc-Regular.gotl         # 向量輪廓，可省略
```

- 每個 `GlyphFont` 都必須恰好出現一次，且至少有一個字型檔。
- 啟動時只檢查檔案存在；字型檔在第一次用到時才讀取。
- 檔案格式見 `io/serializer/format/gfnt-v1.md` 與 `gotl-v1.md`。

## 新增一個 GlyphFont

1. 在 `font.hpp` 的 `enum class GlyphFont` 與 `GLYPH_FONT_NAMES` 各加一筆，名稱即設定檔的 section 名稱。
2. 把字型檔放進 `assets/fonts/`，在 `fonts.ini` 加上對應的 section。

不需要修改其他程式碼。設定檔少了任何一個 `GlyphFont` 時，啟動就會報錯。

## 注意事項

- GLUT bitmap 字型會把非 ASCII 的每個位元組當成 Latin-1 字元畫出；stroke 字型則跳過非 ASCII 位元組。中文請用 GlyphFont。
- 所有字型與點陣化結果在程式結束前都不會釋放，因此字號種類不宜過多。
