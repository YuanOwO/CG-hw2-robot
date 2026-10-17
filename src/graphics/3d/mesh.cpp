#include "graphics/3d/mesh.hpp"

#include <cassert>

#include "graphics/3d/shading.hpp"
#include "platform/glut.hpp"

namespace robot::graphics {

std::uint32_t Mesh::addVertex(const Vec3& position, const Vec3& normal, const ColorRGBA& color) {
    _vertices.push_back({position, normal, color});
    return static_cast<std::uint32_t>(_vertices.size() - 1);
}

void Mesh::addTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    assert(a < _vertices.size() && b < _vertices.size() && c < _vertices.size());

    _indices.push_back(a);
    _indices.push_back(b);
    _indices.push_back(c);
}

void Mesh::draw() const {
    glBegin(GL_TRIANGLES);
    for (std::uint32_t index : _indices) {
        const Vec3& position = _vertices[index].position;
        const Vec3& normal = _vertices[index].normal;
        const ColorRGBA& color = _vertices[index].color;
        const float shade = shadeOf(normal);
        glColor4f(color.r * shade, color.g * shade, color.b * shade, color.a);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(position.x, position.y, position.z);
    }
    glEnd();
}

}  // namespace robot::graphics
