#pragma once

// 呼叫當下直接用 glBegin 畫出的輔助圖形，不產生網格。

namespace robot::graphics {

// 原點出發的 x、y、z 軸，分別為紅、綠、藍
void drawAxes(float length);

}  // namespace robot::graphics
