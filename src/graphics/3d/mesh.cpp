#include "graphics/3d/mesh.hpp"

#include <cassert>

namespace robot::graphics {

std::uint32_t Mesh::addVertex(const Vec3& position, const Vec3& normal, const ColorRGBA& color) {
    // 存入時正規化一次，讀回的法向量一定是單位向量
    _vertices.push_back({position, normalize(normal), color});
    return static_cast<std::uint32_t>(_vertices.size() - 1);
}

void Mesh::addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    assert(a < _vertices.size() && b < _vertices.size() && c < _vertices.size());

    _indices.push_back(a);
    _indices.push_back(b);
    _indices.push_back(c);
}

}  // namespace robot::graphics
