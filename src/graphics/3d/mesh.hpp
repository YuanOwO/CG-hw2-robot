#pragma once

#include <cstdint>
#include <vector>

#include "common/color.hpp"
#include "math/vec3.hpp"

namespace robot::graphics {

/// 三角形網格：頂點只存一次，三角形以編號參照，相鄰的三角形可以共用頂點。
/// 每個頂點帶有自己的顏色，同一個網格可以有多種顏色。
/// 建立一次後可以重複畫，三角形從外側看須為逆時針
class Mesh {
   public:
    /// 網格的一個頂點，位置與法向量屬於網格自己的座標系
    struct Vertex {
        Vec3 position;
        Vec3 normal;  ///< 單位向量
        ColorRGBA color;
    };

    ~Mesh() = default;

    /// 已加入的頂點，順序即 addVertex 回傳的編號
    const std::vector<Vertex>& vertices() const { return _vertices; }

    /// @param position, normal 網格自己座標系中的位置與法向量，法向量須為單位向量
    /// @param color 頂點的基本顏色，畫的時候再依法向量調整明暗
    /// @return 這個頂點的編號，給 addTriangle 使用
    std::uint32_t addVertex(const Vec3& position, const Vec3& normal, const ColorRGBA& color);
    /// 以三個頂點編號加入一個三角形，須為 addVertex 回傳過的編號
    void addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c);

    /// 以目前的 MODELVIEW 畫出網格。顏色取自各頂點，再依法向量決定明暗；
    /// 法向量屬於網格自己的座標系，所以明暗會跟著物體轉
    void draw() const;

   private:
    std::vector<Vertex> _vertices;
    std::vector<std::uint32_t> _indices;
};

}  // namespace robot::graphics
