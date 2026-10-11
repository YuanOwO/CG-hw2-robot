# A Moving Robot

電腦圖學 Homework #2：A Moving Robot。以 C++17、OpenGL 與 GLUT 實作（Windows / Linux 使用 FreeGLUT，macOS 使用系統內建的 GLUT）。

## 建置

需要 CMake 3.16 以上、支援 C++17 的編譯器，以及 OpenGL / GLU / GLUT。

| 平台 | 相依套件 |
| --- | --- |
| macOS | Xcode Command Line Tools（`xcode-select --install`）與 CMake（例如 `brew install cmake`） |
| Linux（Debian / Ubuntu） | `sudo apt install cmake freeglut3-dev libglu1-mesa-dev` |
| Windows | MinGW-w64 + FreeGLUT（MinGW 會靜態連結執行檔） |

> macOS 使用系統內建的 GLUT、OpenGL framework（含 GLU），有硬體加速，不需要 Homebrew 的 FreeGLUT、Mesa 或 XQuartz。Apple GLUT 與 FreeGLUT 的差異（關閉視窗、主循環、修飾鍵、Command 快捷鍵等）由平台層處理，見 [`src/platform/README.md`](src/platform/README.md)。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build build
```

```bash
./build/main
```

建置完成後，`assets/` 會自動複製到執行檔旁邊；程式啟動時從執行檔所在目錄的 `assets/fonts` 載入字型，請保持兩者放在一起。

`Debug` 組態會開啟 `-fsanitize=undefined`。

## 操作

在視窗上按右鍵開啟選單。底部為狀態列。

### 快捷鍵

GLUT 無法使用輸入法，注音等輸入法開啟時單鍵快捷鍵會失效。macOS 上程式在前景時會自動切換為英文鍵盤配置，切換到其他程式或結束時還原；其他平台請自行切換為英文輸入。

| 快捷鍵 | 功能 |
| --- | --- |
| Ctrl+C | 關閉視窗 |

## 專案結構

```text
src/
├── main.cpp
├── app/        Application 與各視窗
├── common/     顏色、座標、矩形與 UTF-8
├── event/      輸入、視窗與元素事件
├── font/       字型樣式、GLUT 字型對應、文字度量與 GFNT 字型
├── input/      輸入狀態與快捷鍵管理
├── io/         檔案讀寫（GFNT 字型等）
├── math/       向量、數學常數與角度轉換、內插等工具函式
├── platform/   GLUT 平台層：FreeGLUT 與 Apple GLUT 的差異
├── render/     繪製流程（Renderer）、相機與投影參數、在介面中嵌入 3D 場景（Scene3DScope）、2D 繪圖（RenderContext）
└── ui/         元素樹、版面配置、選單與主題
assets/fonts/   Cubic 11 與 Unifont 的 GFNT 點陣字型
docs/           報告各章節、圖片與圖片原始檔
```

## 報告

報告主檔為 [`hw2-robot.tex`](hw2-robot.tex)，各章節位於 `docs/`。使用 XeLaTeX 編譯，需安裝 Noto Serif、Noto Serif / Sans CJK TC、Menlo 與 Kaiti TC 字型。

```bash
latexmk -xelatex hw2-robot.tex
```

`docs/figures-src/` 存放圖片原始檔：`.mmd` 由 `render-mermaid.sh` 透過 Mermaid CLI（`mmdc`）輸出為 PNG。

## 授權

`assets/fonts/` 內的字型依各自目錄中的 OFL 授權條款散布。
