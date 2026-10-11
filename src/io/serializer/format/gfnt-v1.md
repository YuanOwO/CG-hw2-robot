# GFNT v1 格式定義

GFNT（Glyph FoNT）是一種自訂的點陣字型格式。字型中的每個字元事先由 TTF / OTF 以固定的像素大小點陣化，存成 8-bit alpha bitmap，讀取端不需要 FreeType 等向量字型引擎，就能直接繪製。

與 [GOTL](gotl-v1.md) 的分工：GFNT 是事先點陣化好的 bitmap，適合小字號（保有 hinting 的品質），讀取端也最簡單；GOTL 一份檔案適用所有字號，適合大字號，或字元數多、無法為每個字號各存一份 bitmap 的字型（例如 CJK）。

本文件定義 GFNT **version 1** 的檔案結構、bitmap 的編碼方式、合法檔案必須滿足的條件，以及字形的定位方式。

## 1. 慣例

- 所有整數皆為 **little-endian**。
- 型別記號：`u16` / `u32` 為無號整數，`i16` 為 two's complement 有號整數，`char[4]` 為 4 個 ASCII 位元組。
- 各結構之間、欄位之間**沒有 padding**，欄位位移即為前面所有欄位大小的總和。多位元組欄位不保證對齊，讀取端應逐位元組組合，或使用允許非對齊存取的讀法。
- 座標與長度的單位皆為**像素**。
- **Y 軸向下為正**，與螢幕座標相同；這與 [GOTL](gotl-v1.md) 相反。`bearingY`、`ascender`、`descender` 則是以基線為準、往上或往下量的**距離**，皆以正數表示，見第 5 節。
- 副檔名為 `.gfnt`。

## 2. 檔案結構

檔案由 Header、Glyph Table、Bitmap Data 三個區段依序緊接組成，不另外記錄區段位移：

| 區段 | 位移 | 大小 |
|---|---|---|
| Header | `0` | 16 bytes |
| Glyph Table | `16` | `18 × N` bytes |
| Bitmap Data | `16 + 18 × N` | 到檔案結尾 |

其中 `N` 為 Header 的 `glyphCount`。

### 2.1 Header（16 bytes）

| 位移 | 型別 | 欄位 | 說明 |
|---:|---|---|---|
| 0 | `char[4]` | `magic` | 固定為 `"GFNT"`（`47 46 4E 54`） |
| 4 | `u16` | `version` | 固定為 `1` |
| 6 | `u16` | `pixelSize` | 點陣化時使用的像素大小（字級），必須大於 0 |
| 8 | `i16` | `ascender` | 基線**上方**的高度，必須 ≥ 0 |
| 10 | `i16` | `descender` | 基線**下方**的深度，以正數表示，必須 ≥ 0 |
| 12 | `u32` | `glyphCount` | Glyph Table 的記錄數 `N` |

> `descender` 與 FreeType / OpenType 慣例不同，**存的是正數**。行高為 `ascender + descender`。

Python `struct` 格式：`<4sHHhhI`。

### 2.2 Glyph Table（每筆 18 bytes）

共 `N` 筆，每筆描述一個字元：

| 位移 | 型別 | 欄位 | 說明 |
|---:|---|---|---|
| 0 | `u32` | `codepoint` | Unicode code point |
| 4 | `u16` | `width` | bitmap 寬度（像素） |
| 6 | `u16` | `height` | bitmap 高度（像素） |
| 8 | `i16` | `bearingX` | pen 位置到 bitmap 左緣的水平距離，向右為正 |
| 10 | `i16` | `bearingY` | 基線到 bitmap 頂端的垂直距離，向上為正 |
| 12 | `u16` | `advanceX` | 畫完此字元後 pen 向右前進的距離 |
| 14 | `u32` | `bitmapOffset` | bitmap 相對於 **Bitmap Data 起點**的位移（bytes） |

Python `struct` 格式：`<IHHhhHI`。

記錄必須依 `codepoint` **嚴格遞增**排列（不可重複），讀取端可直接以二分搜尋查找字元。

- 每個字元只存有墨跡的範圍，`width` / `height` 即為墨跡的 bounding box。
- `width` 或 `height` 為 0 的字元（例如空白）沒有 bitmap，`bitmapOffset` 的值沒有意義，但仍須 ≤ Bitmap Data 的大小。

### 2.3 Bitmap Data

所有字元的 bitmap 存放在此區段，大小為檔案長度減去 `16 + 18 × N`。格式本身不要求 bitmap 依序、不重疊或填滿整個區段；最簡單的寫法是依 Glyph Table 的順序寫入，彼此緊接。

## 3. Bitmap 編碼

每個 bitmap 為 `width × height` bytes，起點為 `16 + 18 × N + bitmapOffset`：

- 每個像素 1 byte，代表 **alpha 覆蓋率**：`0` 為完全透明，`255` 為完全不透明。字型本身不帶顏色，繪製時由讀取端乘上文字顏色。
- 以 **row-major** 存放，列由上往下、每列由左往右，列與列之間沒有 padding。
- 像素 `(x, y)` 位於 `bitmap[y × width + x]`。

```c
const uint8_t *bitmap = bitmapData + bitmapOffset;

for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
        uint8_t alpha = bitmap[y * width + x];
        blend_pixel(dstX + x, dstY + y, textColor, alpha);
    }
}
```

像素值只有 `0` 或 `255` 的單色字型，格式上與灰階字型沒有差別。

## 4. 合法性條件

讀取端應在載入時一次檢查以下條件，任何一項不成立就拒絕整個檔案。以下的加法與乘法都應以 64-bit 計算，或改寫成減法比較，避免溢位：

1. 檔案長度至少 16 bytes，`magic` 為 `"GFNT"`，`version` 為 `1`。
2. `pixelSize ≠ 0`，`ascender ≥ 0`，`descender ≥ 0`。
3. `16 + 18 × glyphCount ≤ 檔案長度`。應先檢查 `glyphCount` 再依它配置記憶體。
4. 對所有 `i ≥ 1`，`glyph[i-1].codepoint < glyph[i].codepoint`。
5. 每個 `codepoint` 都是合法的 Unicode scalar value：`≤ 0x10FFFF`，且不在 `0xD800`–`0xDFFF`（surrogate）。
6. 每個字元的 bitmap 範圍都位於 Bitmap Data 內，即 `bitmapOffset ≤ bitmapDataSize` 且 `width × height ≤ bitmapDataSize − bitmapOffset`。

條件 4–6 只需走訪 Glyph Table 一次，不必讀取 bitmap 內容。通過檢查後，繪製時可直接取用 bitmap，不必每個字元重複檢查邊界。

## 5. Font Metrics 與字形定位

### 5.1 像素座標中的字形

字形以**基線**對齊。繪製時維護一個 pen，表示下一個字元在基線上的起始位置：

```
                  bearingX      width
                 |<------>|<------------->|
                          +---------------+  ---
                          |               |   ^
                          |    bitmap     |   | bearingY
                          |               |   v
  baseline  ------●-------|---------------|--------------●------→ +X
                 pen      |               |             pen'
                          +---------------+
                 |<-------------- advanceX ------------->|     ↓ +Y
```

- 對 pen 位置 `(xp, yb)` 上的字元，bitmap 左上角為：

  ```
  x = xp + bearingX
  y = yb − bearingY
  ```

- `height > bearingY` 時，bitmap 會延伸到基線以下 `height − bearingY` 像素（例如「你」、「g」的下緣）。
- `width` / `height` 為 0 的字元不繪製，只推進 pen。

### 5.2 排版

- 設文字區塊左上角的 Y 座標為 `y0`，第一行基線為 `yb = y0 + ascender`。
- 每畫完一個字元，`xp += advanceX`。
- 每換一行，基線下移 `ascender + descender`（行高），pen 回到左端。

[GOTL](gotl-v1.md) 點陣化後的結果也以本節的欄位描述，兩種格式可以共用同一套排版與繪製邏輯。

## 6. 查找與替代字元

Glyph Table 已排序，查找一個 code point 只需二分搜尋（`O(log N)`）。格式本身不規定缺字時的行為，建議讀取端依序嘗試：

1. 要求的 code point；
2. `U+FFFD`（REPLACEMENT CHARACTER）；
3. `U+003F`（`?`）；
4. 以上都沒有時不繪製，只以 `pixelSize` 推進 pen（即一個 em），讓後續文字維持在正確位置。

## 7. 範例

以下取自 `Cubic11-12.gfnt`（Cubic 11，12 px，10,268 個字元）。

**Header**

```
00000000: 4746 4e54 0100 0c00 0a00 0400 1c28 0000
          └─GFNT──┘ ver=1 size=12 asc=10 desc=4 count=10268
```

Bitmap Data 起點為 `16 + 18 × 10268 = 184840`。

**「你」（U+4F60）的 Glyph 記錄**

```
60 4f 00 00 │ 0d 00 │ 0b 00 │ 00 00 │ 09 00 │ 0d 00 │ 88 d8 01 00
codepoint   │ w=13  │ h=11  │ bx=0  │ by=9  │ adv=13│ offset=120968
```

bitmap 從檔案位移 `184840 + 120968` 開始，共 `13 × 11 = 143` bytes。`bearingY = 9`、`height = 11`，因此最後兩列位於基線以下。

**空白（U+0020）**

```
20 00 00 00 │ 00 00 │ 00 00 │ 00 00 │ 00 00 │ 04 00 │ 00 00 00 00
```

`width = height = 0`，沒有 bitmap，只將 pen 前進 4 px。

## 8. 版本相容性

`version` 欄位用於不相容的結構變更。讀取端遇到不認得的版本必須拒絕載入，不應嘗試以 v1 的結構解析。
