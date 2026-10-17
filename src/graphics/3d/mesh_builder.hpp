#pragma once

#include "common/color.hpp"
#include "graphics/3d/mesh.hpp"
#include "math/curve/path2d.hpp"
#include "math/mat4.hpp"
#include "math/vec3.hpp"

namespace robot::graphics {

/// 把多個形狀組成一個網格。每個形狀先在自己的座標系中建立，再經過目前的變換、帶著目前的顏色放進網格。
/// 旋轉體都繞自己的 y 軸；有底面的形狀以底面中心為原點，往 +y 長，球以球心為原點。
/// 長方體例外，直接以兩個對角決定位置
class MeshBuilder {
   public:
    /// @param segments 圓形的形狀繞一圈切成幾欄，須至少為 3
    /// @param tolerance 剖面切成折線時容許的最大距離，須大於 0
    explicit MeshBuilder(int segments = 32, float tolerance = 0.01f);
    ~MeshBuilder() = default;

    /// 目前累積的網格。builder 可以繼續加入形狀，不影響已取出的網格
    Mesh build() const;

    /// 之後加入的形狀都使用這個顏色，直到下一次呼叫；一開始為白色
    MeshBuilder& setColor(const ColorRGBA& color);

    /// 之後加入的形狀都先經過 mat 再放進網格；取代目前的變換，不與之前的疊加。
    /// 法向量用同一個矩陣變換，所以只支援旋轉、平移與等比例縮放，大小請用各形狀的參數決定
    MeshBuilder& transform(const Mat4& mat);

    /// 以 min 與 max 為對角的長方體，各邊與座標軸平行
    /// @param min, max 各分量都較小與都較大的兩個角；某個分量 min 大於 max 時所有面會朝內
    MeshBuilder& cuboid(const Vec3& min, const Vec3& max);

    /// 球，球心在原點
    MeshBuilder& sphere(float radius);
    /// 膠囊：圓柱的上下各接一個半球，底端在原點
    /// @param height 含兩端半球的總高度，須至少為 2 × radius
    MeshBuilder& capsule(float radius, float height);

    /// 圓柱，底面中心在原點
    MeshBuilder& cylinder(float radius, float height);
    /// 圓錐，底面中心在原點，尖端在 (0, height, 0)
    MeshBuilder& cone(float radius, float height);
    /// 圓台，底面中心在原點；topRadius 為 0 時即圓錐
    MeshBuilder& frustum(float bottomRadius, float topRadius, float height);

    /// 正稜柱，底面中心在原點。每個側面是一片平面，相鄰的面明暗分明
    /// @param radius 外接圓半徑，即中心到頂點的距離；第一個頂點在 +x 方向
    /// @param sides 側面數，須至少為 3
    MeshBuilder& prism(float radius, float height, int sides);
    /// 正角錐，底面中心在原點，尖端在 (0, height, 0)
    /// @param radius 底面的外接圓半徑；第一個頂點在 +x 方向
    /// @param sides 側面數，須至少為 3
    MeshBuilder& pyramid(float radius, float height, int sides);
    /// 正角台，底面中心在原點；topRadius 為 0 時即角錐
    /// @param bottomRadius, topRadius 底面與頂面的外接圓半徑；第一個頂點在 +x 方向
    /// @param sides 側面數，須至少為 3
    MeshBuilder& frustum(float bottomRadius, float topRadius, float height, int sides);

    /// 剖面繞 y 軸轉一圈得到的旋轉體，表面平滑。
    /// 剖面須在 x >= 0 的一側，沿前進方向的右手邊為外側，例如由下往上畫；
    /// 起點或終點落在 y 軸上時，該端自然封口，否則留下開口
    MeshBuilder& lathe(const Path2D& profile);

   private:
    Mesh _mesh;
    Mat4 _transform = Mat4();
    ColorRGBA _currentColor = Color::White;
    int _segments;
    float _tolerance;

    /// lathe 的實作，可指定欄數
    /// @param smooth false 時每個面各自使用面的法向量，用於稜柱等平面組成的形狀；剖面須由直線組成
    void addLathe(const Path2D& profile, int segments, bool smooth = true);
};

}  // namespace robot::graphics
