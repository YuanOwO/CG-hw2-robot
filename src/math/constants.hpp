#pragma once

namespace robot {

// M_PI 不是標準 C++（MSVC 需要 _USE_MATH_DEFINES），因此直接寫出數值
const float PI = 3.14159265358979323846f;  ///< 圓周率
const float EPSILON = 1e-6f;               ///< 浮點數比較的容差值

}  // namespace robot
