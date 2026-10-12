#include "graphics/mesh.hpp"

#include <cassert>

#include "platform/glut.hpp"

namespace robot::graphics {

namespace {

// 各方向的面固定亮度，相鄰的面明暗不同，沒有光照也看得出立體感
const float TOP_SHADE = 1.0f;
const float BOTTOM_SHADE = 0.5f;
const float Z_SHADE = 0.8f;
const float X_SHADE = 0.6f;

// 以法向量分量的平方為權重：平方和為 1，軸對齊的面剛好取到單一方向的亮度
float shadeOf(const Vec3& normal) {
    const float vertical = normal.y > 0.0f ? TOP_SHADE : BOTTOM_SHADE;
    return normal.x * normal.x * X_SHADE + normal.y * normal.y * vertical + normal.z * normal.z * Z_SHADE;
}

}  // namespace

std::uint32_t Mesh::addVertex(const Vec3& position, const Vec3& normal) {
    _positions.push_back(position);
    _normals.push_back(normal);
    return static_cast<std::uint32_t>(_positions.size() - 1);
}

void Mesh::addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    assert(a < _positions.size() && b < _positions.size() && c < _positions.size());

    _indices.push_back(a);
    _indices.push_back(b);
    _indices.push_back(c);
}

void Mesh::draw(const ColorRGBA& color) const {
    glBegin(GL_TRIANGLES);
    for (std::uint32_t index : _indices) {
        const Vec3& position = _positions[index];
        const Vec3& normal = _normals[index];
        const float shade = shadeOf(normal);
        glColor4f(color.r * shade, color.g * shade, color.b * shade, color.a);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(position.x, position.y, position.z);
    }
    glEnd();
}

}  // namespace robot::graphics
