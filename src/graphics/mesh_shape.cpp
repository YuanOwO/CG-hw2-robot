#include "graphics/mesh_shape.hpp"

namespace robot::graphics {

#pragma region Cuboid

void addCuboid(Mesh& mesh, const Vec3& min, const Vec3& max) {
    const Vec3 vertices[] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, min.y, max.z},
        {min.x, min.y, max.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z},
    };

    const Vec3 normals[] = {
        {0.0f,  -1.0f, 0.0f }, // -y
        {0.0f,  1.0f,  0.0f }, // +y
        {0.0f,  0.0f,  -1.0f}, // +z
        {1.0f,  0.0f,  0.0f }, // +x
        {0.0f,  0.0f,  1.0f }, // -z
        {-1.0f, 0.0f,  0.0f }, // -x
    };

    const int faceIndices[][4] = {
        {0, 1, 2, 3}, // -y
        {4, 7, 6, 5}, // +y
        {0, 4, 5, 1}, // +z
        {1, 5, 6, 2}, // +x
        {2, 6, 7, 3}, // -z
        {3, 7, 4, 0}, // -x
    };

    for (int i = 0; i < 6; ++i) {
        const Vec3& normal = normals[i];
        const int* quad = faceIndices[i];

        std::uint32_t a = mesh.addVertex(vertices[quad[0]], normal);
        std::uint32_t b = mesh.addVertex(vertices[quad[1]], normal);
        std::uint32_t c = mesh.addVertex(vertices[quad[2]], normal);
        std::uint32_t d = mesh.addVertex(vertices[quad[3]], normal);

        mesh.addTriangle(a, b, c);
        mesh.addTriangle(a, c, d);
    }
}

Mesh createCuboid(const Vec3& min, const Vec3& max) {
    Mesh mesh;
    addCuboid(mesh, min, max);
    return mesh;
}

#pragma endregion  // Cuboid

}  // namespace robot::graphics
