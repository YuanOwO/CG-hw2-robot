#pragma once

namespace robot {

// M_PI 不是標準 C++（MSVC 需要 _USE_MATH_DEFINES），因此直接寫出數值
constexpr float PI = 3.14159265358979323846f;     ///< 圓周率
constexpr float SQRT_2 = 1.4142135623730950488f;  ///< 根號 2
constexpr float SQRT_3 = 1.7320508075688772935f;  ///< 根號 3
constexpr float EPSILON = 1e-6f;                  ///< 浮點數比較的容差值

}  // namespace robot
