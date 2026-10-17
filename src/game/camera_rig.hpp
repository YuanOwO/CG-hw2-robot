#pragma once

#include "graphics/camera.hpp"
#include "math/vec3.hpp"

namespace robot::game {

/// 第三人稱相機：從目標點沿視線往後退一段距離，轉向時繞著目標點環繞。
/// 相機是遊玩中的操作狀態，不屬於世界；繪製時只取它算出的觀察參數與投影
class CameraRig {
   public:
    void setTarget(const Vec3& target) { _target = target; }

    /// 水平轉向，正值為從上方看逆時針
    void turn(float degree);
    /// 抬頭或低頭，正值抬頭；會停在接近正上方或正下方的地方
    void tilt(float degree);
    /// 沿畫面方向水平移動目標點，畫面上的移動速度不隨縮放改變
    /// @param forward, right 倍率為 1 時的移動距離，公尺
    void pan(float forward, float right);
    /// @param factor 大於 1 拉近、小於 1 拉遠
    void zoom(float factor);

    /// 度，[0, 360)；為 0 時看向 -z
    float yaw() const { return _yaw; }
    /// 度，正值抬頭
    float pitch() const { return _pitch; }

    graphics::Camera camera() const;
    graphics::OrthoProjection projection() const;

   private:
    Vec3 _target;

    // yaw 為 0 時看向 -z，與 OpenGL 眼睛座標的預設方向一致
    float _yaw = 45.0f;
    float _pitch = -20.0f;
    float _distance = 75.0f;  // 公尺，只決定眼睛的位置，不影響畫面大小

    // 平行投影沒有近大遠小，縮放要縮小 view volume，改 _distance 沒有效果
    float _zoom = 4.0f;
};

}  // namespace robot::game
