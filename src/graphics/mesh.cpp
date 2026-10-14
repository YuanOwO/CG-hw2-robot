#include "graphics/mesh.hpp"

#include <cassert>

#include "graphics/shading.hpp"
#include "platform/glut.hpp"

namespace robot::graphics {

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
