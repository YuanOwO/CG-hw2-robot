#pragma once

#include <cstdint>
#include <vector>

#include "common/color.hpp"
#include "math/vec3.hpp"

namespace robot::graphics {

/// 三角形網格：頂點只存一次，三角形以編號參照，相鄰的三角形可以共用頂點。
/// 建立一次後可以重複畫，三角形從外側看須為逆時針
class Mesh {
   public:
    ~Mesh() = default;

    /// @param position, normal 網格自己座標系中的位置與法向量
    /// @return 這個頂點的編號，給 addTriangle 使用
    std::uint32_t addVertex(const Vec3& position, const Vec3& normal);
    /// 以三個頂點編號加入一個三角形，須為 addVertex 回傳過的編號
    void addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c);

    /// 以目前的 MODELVIEW 畫出網格，依法向量決定各面明暗；
    /// 法向量屬於網格自己的座標系，所以明暗會跟著物體轉
    void draw(const ColorRGBA& color) const;

   private:
    std::vector<Vec3> _positions;
    std::vector<Vec3> _normals;
    std::vector<std::uint32_t> _indices;
};

}  // namespace robot::graphics
