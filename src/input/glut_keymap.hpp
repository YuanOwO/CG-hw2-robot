#pragma once

#include "input/input_types.hpp"

namespace robot {

// 把 GLUT 的按鍵與滑鼠代碼換成 Key / MouseButton；認不得的回傳 Unknown。

// glutKeyboardFunc 的字元。按住 Ctrl 時 GLUT 送的是控制字元，要另外換算
Key fromGlutKeyboard(unsigned char key, bool ctrlDown);
// glutSpecialFunc 的代碼，包含 platform 補上的修飾鍵
Key fromGlutSpecial(int key);
MouseButton fromGlutMouseButton(int button);

}  // namespace robot
