# GOTL v1 格式定義

GOTL（Glyph OuTLine）是一種自訂的向量字型格式。字型中每個字元的輪廓事先由 TTF / OTF 解析出來，整理成只含 `MoveTo`、`LineTo`、`CurveTo` 三種指令的路徑，讀取端不需要解析 TrueType / CFF，就能在任意字號下點陣化或直接以 GPU 繪製。

與 [GFNT](gfnt-v1.md)（Glyph FoNT）的分工：GFNT 是事先點陣化好的 bitmap，適合小字號（保有 hinting 的品質），讀取端也最簡單；GOTL 一份檔案適用所有字號，適合大字號，或字元數多、無法為每個字號各存一份 bitmap 的字型（例如 CJK）。

本文件定義 GOTL **version 1** 的檔案結構、輪廓的編碼方式、合法檔案必須滿足的條件，以及如何點陣化成與 GFNT 相同的字形資料。

## 1. 慣例

- 所有整數皆為 **little-endian**。
- 型別記號：`u8` / `u16` / `u32` 為無號整數，`i16` 為 two's complement 有號整數，`char[4]` 為 4 個 ASCII 位元組。
- 所有多位元組欄位都位於其大小的**自然對齊**位置（`u16` / `i16` 在偶數位移、`u32` 在 4 的倍數位移），讀取端可以直接以指標存取，不必逐位元組組合。結構內需要的 padding 會明確列出。
- 座標與長度的單位皆為 **font units**，換算成像素時乘上 `pixelSize / unitsPerEm`，見第 5 節。
- **Y 軸向上為正**，原點為 pen 在基線上的位置。這與字型原始資料及 OpenGL 的慣例相同，但**與 GFNT 相反**（GFNT 的像素座標 Y 向下）。`ascender` / `descender` 則沿用 GFNT 的慣例，皆以正數表示距離。
- 副檔名為 `.gotl`。

## 2. 檔案結構

檔案由 Header、Glyph Table、Outline Data 三個區段組成。後兩者的位置與大小記錄在 Header 中：

| 區段 | 位移 | 大小 |
|---|---|---|
| Header | `0` | 32 bytes |
| Glyph Table | `glyphTableOffset` | `20 × N` bytes |
| Outline Data | `outlineDataOffset` | `outlineDataSize` bytes |

其中 `N` 為 Header 的 `glyphCount`。最簡單的配置是三個區段依序緊接，即 `glyphTableOffset = 32`、`outlineDataOffset = 32 + 20 × N`，且檔案結束於 Outline Data 的結尾；但讀取端應一律使用 Header 中的位移，不可自行計算。

### 2.1 Header（32 bytes）

| 位移 | 型別 | 欄位 | 說明 |
|---:|---|---|---|
| 0 | `char[4]` | `magic` | 固定為 `"GOTL"`（`47 4F 54 4C`） |
| 4 | `u16` | `version` | 固定為 `1` |
| 6 | `u16` | `unitsPerEm` | 每個 em 的 font units 數，必須大於 0 |
| 8 | `i16` | `ascender` | 基線**上方**的高度，必須 ≥ 0 |
| 10 | `i16` | `descender` | 基線**下方**的深度，以正數表示，必須 ≥ 0 |
| 12 | `u8` | `curveOrder` | 檔案中所有 `CurveTo` 的次數：`2`（二次貝茲）或 `3`（三次貝茲） |
| 13 | `u8` | `reserved` | 必須為 `0` |
| 14 | `u16` | `flags` | v1 必須為 `0`，保留給日後的編碼選項 |
| 16 | `u32` | `glyphCount` | Glyph Table 的記錄數 `N` |
| 20 | `u32` | `glyphTableOffset` | Glyph Table 在檔案中的位移，必須是 4 的倍數 |
| 24 | `u32` | `outlineDataOffset` | Outline Data 在檔案中的位移，必須是 4 的倍數 |
| 28 | `u32` | `outlineDataSize` | Outline Data 的大小（bytes） |

> `descender` 與 FreeType / OpenType 慣例不同，**存的是正數**。行高為 `ascender + descender`。

Python `struct` 格式：`<4sHHhhBBHIIII`。

### 2.2 Glyph Table（每筆 20 bytes）

共 `N` 筆，每筆描述一個字元：

| 位移 | 型別 | 欄位 | 說明 |
|---:|---|---|---|
| 0 | `u32` | `codepoint` | Unicode code point |
| 4 | `u32` | `outlineOffset` | 輪廓相對於 **Outline Data 起點**的位移（bytes），必須是偶數 |
| 8 | `u16` | `opCount` | 輪廓的指令數，`0` 表示沒有輪廓（例如空白） |
| 10 | `u16` | `advanceX` | 畫完此字元後 pen 向右前進的距離 |
| 12 | `i16` | `xMin` | 輪廓所有點的最小 X |
| 14 | `i16` | `yMin` | 輪廓所有點的最小 Y |
| 16 | `i16` | `xMax` | 輪廓所有點的最大 X |
| 18 | `i16` | `yMax` | 輪廓所有點的最大 Y |

Python `struct` 格式：`<IIHHhhhh`。

記錄必須依 `codepoint` **嚴格遞增**排列（不可重複），讀取端可直接以二分搜尋查找字元。

- **Bounding box** 是輪廓中所有點（**包含控制點**）座標的最小值與最大值，必須完全相等，不可放寬。由於貝茲曲線必定落在其控制點的凸包內，整個輪廓保證位於 bbox 中，點陣化時可以據此配置 bitmap，不必另外裁切。
- `opCount = 0` 時，bbox 四個欄位都必須為 `0`。`outlineOffset` 的值沒有意義，但仍須 ≤ `outlineDataSize`。
- 多筆記錄可以指向同一個輪廓（相同的 `outlineOffset` 與 `opCount`），例如相容漢字與其統一漢字共用字形時。此時它們的 bbox 必然相同，`advanceX` 則各自記錄。

### 2.3 Outline Data

所有輪廓存放在此區段。格式本身不要求輪廓依序、不重疊或填滿整個區段；最簡單的寫法是依 Glyph Table 的順序、在每個字形第一次出現時寫入，彼此緊接。

## 3. 輪廓編碼

### 3.1 結構

每個輪廓由以下三部分緊接組成，起點為 `outlineDataOffset + outlineOffset`：

| 部分 | 大小 | 說明 |
|---|---|---|
| `ops` | `opCount` bytes | 每個指令 1 byte |
| padding | `opCount mod 2` bytes | 值為 `0`，讓座標落在偶數位移 |
| `points` | `4 × pointCount` bytes | 每個點為 `i16 x, i16 y` |

`pointCount` 不另外記錄，由 `ops` 推算：

| 值 | 指令 | 點數 | 點的意義（依序） |
|---:|---|---:|---|
| `0` | `MoveTo` | 1 | 新輪廓的起點 |
| `1` | `LineTo` | 1 | 線段終點 |
| `2` | `CurveTo` | `curveOrder` | 二次：控制點、終點；三次：控制點 1、控制點 2、終點 |

讀取端依序走訪 `ops`，每個指令從 `points` 取出對應數量的點。

```c
const uint8_t *ops = outline;
const int16_t *pt  = (const int16_t *)(outline + opCount + (opCount & 1));

for (uint16_t i = 0; i < opCount; i++) {
    switch (ops[i]) {
    case 0: move_to(pt[0], pt[1]); pt += 2; break;
    case 1: line_to(pt[0], pt[1]); pt += 2; break;
    case 2:
        if (curveOrder == 2) { quad_to(pt[0], pt[1], pt[2], pt[3]);                   pt += 4; }
        else                 { cubic_to(pt[0], pt[1], pt[2], pt[3], pt[4], pt[5]);    pt += 6; }
        break;
    }
}
close_contour();  /* 最後一個輪廓同樣要閉合，見 3.2 */
```

### 3.2 輪廓與曲線

- 第一個指令必須是 `MoveTo`。每個 `MoveTo` 開始一個新的輪廓。
- **輪廓一律是封閉的**，格式中沒有 `Close` 指令：遇到下一個 `MoveTo` 或輪廓資料結尾時，從目前位置畫一條直線回到該輪廓的起點。若目前位置已經在起點上，這條閉合線段長度為 0，可以忽略。
- 每個 `LineTo` / `CurveTo` 的起點是前一個指令的終點（記為 `P0`）。曲線為：
  - 二次（控制點 `P1`、終點 `P2`）：`B(t) = (1−t)² P0 + 2t(1−t) P1 + t² P2`
  - 三次（控制點 `P1`、`P2`，終點 `P3`）：`B(t) = (1−t)³ P0 + 3t(1−t)² P1 + 3t²(1−t) P2 + t³ P3`
- 所有 on-curve 點都明確寫出。與 TrueType 不同，GOTL 沒有「兩個相鄰控制點之間隱含一個 on-curve 點」的規則：每個 `CurveTo` 都是完整的一段曲線。

### 3.3 填色規則

字形的內部以 **nonzero winding** 規則決定：從一點往 +X 方向射出一條水平射線，每遇到一段向上（+Y）穿過射線的輪廓加 1，向下穿過的減 1，總和（繞數）不為 0 的點位於字形內部。

- 格式不規定輪廓的方向。外框與洞的方向相反即可，順時針或逆時針都正確（TrueType 字型的外框通常是順時針、CFF 是逆時針）。
- 輪廓可以互相重疊或交叉，例如筆畫拆成數個重疊的輪廓。這種情況下**不可使用 even-odd 規則**，否則重疊處會被挖空。

## 4. 合法性條件

讀取端應在載入時一次檢查以下條件，任何一項不成立就拒絕整個檔案。以下的加法與乘法都應以 64-bit 計算，或改寫成減法比較，避免溢位：

1. 檔案長度至少 32 bytes，`magic` 為 `"GOTL"`，`version` 為 `1`。
2. `unitsPerEm ≠ 0`，`ascender ≥ 0`，`descender ≥ 0`，`curveOrder` 為 `2` 或 `3`，`reserved = 0`，`flags = 0`。
3. `glyphTableOffset` 與 `outlineDataOffset` 都是 4 的倍數且 ≥ 32。
4. `glyphTableOffset + 20 × glyphCount ≤ 檔案長度`，`outlineDataOffset + outlineDataSize ≤ 檔案長度`，且這兩個區段不重疊。應先檢查 `glyphCount` 再依它配置記憶體。
5. 對所有 `i ≥ 1`，`glyph[i-1].codepoint < glyph[i].codepoint`。
6. 每個 `codepoint` 都是合法的 Unicode scalar value：`≤ 0x10FFFF`，且不在 `0xD800`–`0xDFFF`（surrogate）。
7. `opCount = 0` 的字元：`outlineOffset ≤ outlineDataSize`，bbox 四個欄位皆為 `0`。
8. `opCount > 0` 的字元：
   1. `outlineOffset` 為偶數，且 `opCount + (opCount mod 2) ≤ outlineDataSize − outlineOffset`（先確認 `outlineOffset ≤ outlineDataSize`）。
   2. `ops[0] = 0`（`MoveTo`），所有 `ops[i]` 都是 `0`、`1` 或 `2`。
   3. 依 3.1 算出 `pointCount` 後，`4 × pointCount ≤ outlineDataSize − outlineOffset − opCount − (opCount mod 2)`。
   4. 所有點的 X 的最小值、最大值分別等於 `xMin`、`xMax`，Y 亦同。

條件 8 需要走訪每個輪廓一次，成本與檔案大小成正比（Noto Sans CJK 約 500 萬個點）。共用同一輪廓的記錄可以只檢查一次。通過檢查後，繪製時可直接取用輪廓，不必再檢查邊界。

## 5. Font Metrics、字形定位與點陣化

### 5.1 Font units 中的字形

```
                  xMin               xMax
                   |                  |
          yMax  ---+------------------+
                   |                  |
                   |     outline      |      ↑ +Y
                   |                  |      |
  baseline  ●------|------------------|------●------→ +X
           pen     |                  |     pen'
          yMin  ---+------------------+
           |<----------------- advanceX ---->|
```

- pen 位於原點 `(0, 0)`，輪廓座標直接以 pen 為基準，不需要另外的 bearing。
- 畫完後 pen 向右前進 `advanceX`。
- 行高為 `ascender + descender`。

### 5.2 換算成 GFNT 的字形資料

給定字號 `pixelSize`，令 `s = pixelSize / unitsPerEm`。點陣化結果可以用 GFNT 的欄位描述（見 [GFNT v1 第 5 節](gfnt-v1.md#5-font-metrics-與字形定位)），排版與繪製邏輯與 GFNT 完全共用：

| GFNT 欄位 | 由 GOTL 計算 |
|---|---|
| `ascender` | `ceil(ascender × s)` |
| `descender` | `ceil(descender × s)` |
| `bearingX` | `floor(xMin × s)` |
| `bearingY` | `ceil(yMax × s)` |
| `width` | `ceil(xMax × s) − floor(xMin × s)` |
| `height` | `ceil(yMax × s) − floor(yMin × s)` |
| `advanceX` | `round(advanceX × s)` |

`opCount = 0` 的字元 `width = height = 0`，只推進 pen。讀取端也可以保留小數的 pen 位置，累加未取整的 `advanceX × s`，讓長字串的寬度更精確；這是排版上的選擇，與格式無關。

### 5.3 點陣化

輪廓上的點 `(x, y)` 轉成 bitmap 中的像素座標 `(u, v)`（`v` 向下，第一列為最上面一列，與 GFNT 相同）：

```
u = x × s − bearingX
v = bearingY − y × s
```

由於 bbox 包含所有控制點，轉換後的點都位於 `[0, width] × [0, height]` 內。浮點誤差可能讓結果略為超出，實作時應夾到範圍內。

每個像素的值為該像素面積中，依 3.3 的 nonzero 規則位於字形內部的比例，乘以 255 後四捨五入。實作可以近似此值，常見的做法：

- 將曲線細分成線段（每段在像素空間中約 0.5 px 以內的誤差即可），再以有號面積累加，最後逐列取前綴和得到每個像素的繞數，覆蓋率取 `min(|累加值|, 1)`。此法自然得到反鋸齒效果。
- 以 GPU 的 stencil buffer 計算繞數（stencil-then-cover），邊緣以 MSAA 反鋸齒。`curveOrder = 2` 時，曲線部分可以在 fragment shader 中以 `u² − v < 0` 判斷（Loop–Blinn），這是選擇二次曲線的主要理由。

GOTL 不含 hinting 資訊，12–16 px 以下的小字會比 FreeType 點陣化的結果模糊，這類字號建議改用 GFNT。

## 6. 查找與替代字元

Glyph Table 已排序，查找一個 code point 只需二分搜尋（`O(log N)`）。格式本身不規定缺字時的行為，建議讀取端依序嘗試：

1. 要求的 code point；
2. `U+FFFD`（REPLACEMENT CHARACTER）；
3. `U+003F`（`?`）；
4. 以上都沒有時不繪製，只以 `unitsPerEm` 推進 pen（即一個 em，換算成像素為 `pixelSize`），讓後續文字維持在正確位置。

## 7. 範例

以下取自 `curveOrder = 3` 的 `NotoSansCJKtc-Regular.gotl`（Noto Sans CJK TC Regular，44,810 個字元）。

**Header**

```
00000000: 474f 544c 0100 e803 8804 2001 0300 0000
          └─GOTL──┘ ver=1 upem=1000 asc=1160 desc=288 order=3 reserved=0 flags=0
00000010: 0aaf 0000 2000 0000 e8ac 0d00 5e11 1b01
          count=44810 glyphTable=32 outlineData=896232 outlineSize=18551134
```

`32 + 20 × 44810 = 896232`，三個區段依序緊接，檔案大小為 `896232 + 18551134 = 19447366` bytes。

**「口」（U+53E3）的 Glyph 記錄**

```
e3 53 00 00 │ 1a 79 03 00 │ 0c 00 │ e8 03 │ 7f 00 │ c9 ff │ 6c 03 │ df 02
codepoint   │ off=227610  │ ops=12│adv=1000│xMin=127│yMin=-55│xMax=876│yMax=735
```

輪廓從檔案位移 `896232 + 227610` 開始：12 bytes 的 `ops`（偶數，不需 padding），接著 12 個點共 48 bytes。

```
ops:    00 01 01 01 01 01 01 01 │ 00 01 01 01
points: M (127,735)  L (127,-55)  L (205,-55)  L (205,30)
        L (796,30)   L (796,-51)  L (876,-51)  L (876,735)     ← 外框，逆時針，閉合回 (127,735)
        M (205,107)  L (205,660)  L (796,660)  L (796,107)     ← 內框（洞），順時針，閉合回 (205,107)
```

外框與內框方向相反，洞內的點繞數為 `+1 − 1 = 0`，位於字形外部。外框下緣的 `(127,-55)`、`(876,-51)` 是「口」兩側豎筆突出到基線以下的部分。

以 24 px 點陣化時，`s = 0.024`：`bearingX = floor(3.048) = 3`、`bearingY = ceil(17.64) = 18`、`width = ceil(21.024) − 3 = 19`、`height = 18 − floor(−1.32) = 20`、`advanceX = 24`。

**「o」（U+006F）**

```
ops:    00 02 02 02 02 │ 00 02 02 02 02
points: M (303,-13)
        C (436,-13) (554,91)  (554,271)
        C (554,452) (436,557) (303,557)
        C (170,557) (52,452)  (52,271)
        C (52,91)   (170,-13) (303,-13)     ← 終點與起點相同，閉合線段長度為 0
        M (303,63)  ...                     ← 內圈，4 段曲線，順時針
```

每個 `CurveTo` 取 3 個點（`curveOrder = 3`）。若以二次曲線（`curveOrder = 2`）表示，同一個字會變成 2 個 `MoveTo` 加 21 個 `CurveTo`。

**空白（U+0020）**

```
20 00 00 00 │ 00 00 00 00 │ 00 00 │ e0 00 │ 00 00 │ 00 00 │ 00 00 │ 00 00
```

`opCount = 0`，沒有輪廓，只將 pen 前進 224 font units（24 px 時為 `round(5.376) = 5` px）。

## 8. 版本相容性

`version` 欄位用於不相容的結構變更，`flags` 用於 v1 結構下的可選編碼方式（例如日後的座標差值壓縮）。讀取端遇到不認得的版本，或任何不認得的 `flags` 位元，都必須拒絕載入，不應嘗試以 v1 的結構解析。
