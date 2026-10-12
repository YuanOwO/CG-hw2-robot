#pragma once

#include <cstdint>
#include <vector>

#include "common/color.hpp"
#include "math/vec3.hpp"

namespace robot::graphics {

// 三角形網格：頂點只存一次，三角形以編號參照，相鄰的三角形可以共用頂點。
// 三角形從外側看須為逆時針。
class Mesh {
   public:
    std::uint32_t addVertex(const Vec3& position, const Vec3& normal);  // 回傳這個頂點的編號
    void addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c);

    // 不開燈，依法向量決定各面明暗。法向量是網格自己的座標系，所以明暗會跟著物體轉
    void draw(const ColorRGBA& color) const;

   private:
    std::vector<Vec3> _positions;
    std::vector<Vec3> _normals;
    std::vector<std::uint32_t> _indices;
};

}  // namespace robot::graphics
