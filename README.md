# A Moving Robot

電腦圖學 Homework #2：A Moving Robot。以 C++17、OpenGL 與 GLUT 實作（Windows / Linux 使用 FreeGLUT，macOS 使用系統內建的 GLUT）。

## 建置

需要 CMake 3.16 以上、支援 C++17 的編譯器，以及 OpenGL / GLU / GLUT。

| 平台                     | 相依套件                                                                                  |
| ------------------------ | ----------------------------------------------------------------------------------------- |
| macOS                    | Xcode Command Line Tools（`xcode-select --install`）與 CMake（例如 `brew install cmake`） |
| Linux（Debian / Ubuntu） | `sudo apt install cmake freeglut3-dev libglu1-mesa-dev`                                   |
| Windows                  | MinGW-w64 + FreeGLUT（MinGW 會靜態連結執行檔）                                            |

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

啟動後為主選單，選「開始」進入世界。世界目前是一片 64×64 的棋盤格草地，機器人尚在建置中。

| 按鍵           | 功能                                     |
| -------------- | ---------------------------------------- |
| 左鍵拖曳       | 遊玩中：左右轉動、上下俯仰視角           |
| W / A / S / D  | 遊玩中：沿畫面方向在水平面上移動視角     |
| Q / E          | 遊玩中：繞注視點環繞視角                 |
| = / -          | 遊玩中：拉近 / 拉遠                      |
| Esc            | 遊玩中：打開暫停選單；選單中：返回上一層 |
| ↑ ↓ / Tab      | 在選單中選擇按鈕                         |
| Enter / 空白鍵 | 按下選擇的按鈕                           |

選單也可以用滑鼠點擊。

GLUT 無法使用輸入法，注音等輸入法開啟時單鍵按鍵會失效。macOS 上程式在前景時會自動切換為英文鍵盤配置，切換到其他程式或結束時還原；其他平台請自行切換為英文輸入。

## 專案結構

```text
src/
├── main.cpp
├── engine/     視窗、主迴圈（60 Hz 固定 tick）、輸入狀態
├── game/       遊戲流程（Session 與畫面的切換），組裝其他模組
├── world/      世界的狀態與規則，不涉及繪製與輸入
├── view/       依世界狀態繪製場景與 HUD
├── screen/     選單畫面（主選單、暫停、操作說明）
├── gui/        通用介面元件（按鈕）
├── graphics/   不認識世界的繪圖工具：2D 畫布、座標軸、繪製階段、相機與投影參數、三角形網格與長方體
├── common/     顏色、座標、矩形與 UTF-8
├── font/       字型的量測與排版（見 src/font/README.md）
├── input/      按鍵代碼與輸入狀態
├── io/         檔案讀寫（字型、設定檔）
├── math/       向量、數學常數與角度轉換、內插等工具函式
└── platform/   GLUT 平台層：FreeGLUT 與 Apple GLUT 的差異
assets/fonts/   字型檔與字型設定（fonts.ini）
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
