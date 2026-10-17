#pragma once

#include <algorithm>
#include <string>

namespace robot {

// 以 '\n' 分行的行數。結尾換行不產生額外的空白行，與 FreeGLUT 的行為一致，
// 所有字型的高度量測都以這裡為準。
inline int countLines(const std::string& text) {
    if (text.empty()) {
        return 0;
    }

    int lines = static_cast<int>(std::count(text.begin(), text.end(), '\n')) + 1;
    if (text.back() == '\n') {
        lines--;
    }
    return lines;
}

}  // namespace robot
