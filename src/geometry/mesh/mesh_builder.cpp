#include "geometry/mesh/mesh_builder.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "math/constants.hpp"
#include "math/mat4.hpp"
#include "math/utils.hpp"

namespace robot::geometry {

MeshBuilder::MeshBuilder(int segments, float tolerance) : _segments(segments), _tolerance(tolerance) {}

Mesh MeshBuilder::build() const {
    return _mesh;
}

MeshBuilder& MeshBuilder::setColor(const ColorRGBA& color) {
    _currentColor = color;
    return *this;
}

MeshBuilder& MeshBuilder::transform(const Mat4& mat) {
    _transform = mat;
    return *this;
}

#pragma region Cuboid

MeshBuilder& MeshBuilder::cuboid(const Vec3& min, const Vec3& max) {
    // 0～3 為底面、4～7 為頂面，各自依相同順序繞一圈，i 與 i + 4 上下對齊
    Vec3 vertices[] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, min.y, max.z},
        {min.x, min.y, max.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z},
    };

    for (int i = 0; i < 8; ++i) {
        vertices[i] = transformPoint(_transform, vertices[i]);
    }

    Vec3 normals[] = {
        {0.0f,  -1.0f, 0.0f }, // -y
        {0.0f,  1.0f,  0.0f }, // +y
        {0.0f,  0.0f,  -1.0f}, // -z
        {1.0f,  0.0f,  0.0f }, // +x
        {0.0f,  0.0f,  1.0f }, // +z
        {-1.0f, 0.0f,  0.0f }, // -x
    };

    for (int i = 0; i < 6; ++i) {
        normals[i] = normalize(transformVector(_transform, normals[i]));
    }

    // 每個面的四個角，從外側看為逆時針；順序與 normals 對應
    const int faceIndices[][4] = {
        {0, 1, 2, 3}, // -y
        {4, 7, 6, 5}, // +y
        {0, 4, 5, 1}, // -z
        {1, 5, 6, 2}, // +x
        {2, 6, 7, 3}, // +z
        {3, 7, 4, 0}, // -x
    };

    // 每個面各存四個頂點，相鄰的面在稜線上不共用，法向量才能各自不同
    for (int i = 0; i < 6; ++i) {
        const Vec3& normal = normals[i];
        const int* quad = faceIndices[i];

        std::uint32_t a = _mesh.addVertex(vertices[quad[0]], normal, _currentColor);
        std::uint32_t b = _mesh.addVertex(vertices[quad[1]], normal, _currentColor);
        std::uint32_t c = _mesh.addVertex(vertices[quad[2]], normal, _currentColor);
        std::uint32_t d = _mesh.addVertex(vertices[quad[3]], normal, _currentColor);

        _mesh.addTriangle(a, b, c);
        _mesh.addTriangle(a, c, d);
    }

    return *this;
}

#pragma endregion  // Cuboid

#pragma region Sphere, Capsule

MeshBuilder& MeshBuilder::sphere(float radius) {
    Path2D profile =
        Path2D(Vec2(0.0f, -radius)).arcTo(Vec2(0.0f, 0.0f), Vec2(0.0f, radius), ArcDirection::CounterClockwise);
    addLathe(profile, _segments);
    return *this;
}

MeshBuilder& MeshBuilder::capsule(float radius, float height) {
    float cylinderHeight = height - 2 * radius;
    assert(cylinderHeight >= 0.0f);

    Path2D profile =
        Path2D(Vec2(0.0f, 0.0f))
            .arcTo(Vec2(0.0f, radius), Vec2(radius, radius), ArcDirection::CounterClockwise)
            .lineTo(Vec2(radius, radius + cylinderHeight))
            .arcTo(Vec2(0.0f, radius + cylinderHeight), Vec2(0.0f, height), ArcDirection::CounterClockwise);
    addLathe(profile, _segments);
    return *this;
}

#pragma endregion  // Sphere, Capsule

#pragma region Cylinder, Cone

MeshBuilder& MeshBuilder::cylinder(float radius, float height) {
    return frustum(radius, radius, height);
}

MeshBuilder& MeshBuilder::cone(float radius, float height) {
    return frustum(radius, 0.0f, height);
}

MeshBuilder& MeshBuilder::frustum(float bottomRadius, float topRadius, float height) {
    Path2D profile = Path2D(Vec2(0.0f, 0.0f));
    if (bottomRadius > EPSILON) {
        profile = profile.lineTo(Vec2(bottomRadius, 0.0f));
    }
    profile = profile.lineTo(Vec2(topRadius, height));
    if (topRadius > EPSILON) {
        profile = profile.lineTo(Vec2(0.0f, height));
    }
    addLathe(profile, _segments);
    return *this;
}

#pragma endregion  // Cylinder, Cone

#pragma region Prism, Pyramid

MeshBuilder& MeshBuilder::prism(float radius, float height, int sides) {
    return frustum(radius, radius, height, sides);
}

MeshBuilder& MeshBuilder::pyramid(float radius, float height, int sides) {
    return frustum(radius, 0.0f, height, sides);
}

MeshBuilder& MeshBuilder::frustum(float bottomRadius, float topRadius, float height, int sides) {
    Path2D profile = Path2D(Vec2(0.0f, 0.0f));
    if (bottomRadius > EPSILON) {
        profile = profile.lineTo(Vec2(bottomRadius, 0.0f));
    }
    profile = profile.lineTo(Vec2(topRadius, height));
    if (topRadius > EPSILON) {
        profile = profile.lineTo(Vec2(0.0f, height));
    }
    addLathe(profile, sides, false);
    return *this;
}

#pragma endregion  // Prism, Pyramid

MeshBuilder& MeshBuilder::lathe(const Path2D& profile) {
    addLathe(profile, _segments);
    return *this;
}

MeshBuilder& MeshBuilder::triangle(const Vec3& a, const Vec3& b, const Vec3& c) {
    const Vec3 ta = transformPoint(_transform, a);
    const Vec3 tb = transformPoint(_transform, b);
    const Vec3 tc = transformPoint(_transform, c);

    // 法向量由變換後的頂點算出，所以不必另外變換
    const Vec3 normal = cross(tb - ta, tc - ta);
    std::uint32_t ia = _mesh.addVertex(ta, normal, _currentColor);
    std::uint32_t ib = _mesh.addVertex(tb, normal, _currentColor);
    std::uint32_t ic = _mesh.addVertex(tc, normal, _currentColor);
    _mesh.addTriangle(ia, ib, ic);
    return *this;
}

#pragma region Lathe Implementation

void MeshBuilder::addLathe(const Path2D& profile, int segments, bool smooth) {
    // 剖面須在 x >= 0 的一側，前進方向的右手邊為外側，三角形從外側看才是逆時針。
    // 兩端落在軸上時那一圈縮成一點，自然封口

    assert(segments >= 3);  // 少於三欄圍不出有體積的形狀

    auto vertices = profile.flatten(_tolerance);
    if (vertices.size() < 2) {
        return;  // 沒有有效的剖面
    }

    std::vector<std::vector<std::uint32_t>> ringIndices(vertices.size());  // 每個剖面點轉一圈得到的頂點編號

    Vec3 Y(0.0f, 1.0f, 0.0f);                // y 軸方向
    std::vector<Mat4> vTransform(segments);  // 第 j 欄的位置變換
    std::vector<Mat4> nTransform(segments);  // 第 j 欄的法向量變換

    float halfAngle = 180.0f / segments;  // 相鄰兩欄夾角的一半
    Mat4 fixNormalY;
    if (!smooth) {
        // 平面的面在中間角度只到弦的中點，離軸 x cos(halfAngle)，高度不變，
        // 斜面因此比剖面陡：法向量的 y 分量要乘上 cos(halfAngle)
        fixNormalY = scale(1.0f, std::cos(radians(halfAngle)), 1.0f);
    }

    for (int i = 0; i < segments; i++) {
        // 先在自己的座標系裡繞 y 軸轉，再套用 _transform 擺到位置上。
        // 平滑時法向量用頂點所在的角度；不平滑時用整個面的法向量，也就是兩欄中間的角度
        float vertexDegree = 360.0f * i / segments;
        float normalDegree = vertexDegree + halfAngle * (!smooth);
        vTransform[i] = _transform * rotation(vertexDegree, Y);
        nTransform[i] = _transform * fixNormalY * rotation(normalDegree, Y);
    }

    for (std::size_t i = 0; i < vertices.size(); i++) {
        Vec3 p(vertices[i].point.x, vertices[i].point.y, 0.0f);  // 剖面放在 xy 平面上
        Vec3 n(vertices[i].normal.x, vertices[i].normal.y, 0.0f);

        for (int j = 0; j < segments; j++) {
            Vec3 p_trans = transformPoint(vTransform[j], p);
            Vec3 n_trans = normalize(transformVector(nTransform[j], n));

            if (smooth) {
                ringIndices[i].push_back(_mesh.addVertex(p_trans, n_trans, _currentColor));
            } else {
                // 每個面各存左右兩個頂點，相鄰的面在稜線上不共用，法向量才能各自不同
                ringIndices[i].push_back(_mesh.addVertex(p_trans, n_trans, _currentColor));
                Vec3 p_next = transformPoint(vTransform[(j + 1) % segments], p);
                ringIndices[i].push_back(_mesh.addVertex(p_next, n_trans, _currentColor));
            }
        }
    }

    for (std::size_t i = 1; i < ringIndices.size(); i++) {
        for (std::size_t j = 0; j < ringIndices[i].size(); j++) {
            std::size_t j1 = (j + 1) % ringIndices[i].size();  // 最後一欄接回第一欄
            std::uint32_t a = ringIndices[i - 1][j];
            std::uint32_t b = ringIndices[i - 1][j1];
            std::uint32_t c = ringIndices[i][j];
            std::uint32_t d = ringIndices[i][j1];

            Vec3 pa = _mesh.vertices()[a].position;
            Vec3 pb = _mesh.vertices()[b].position;
            Vec3 pc = _mesh.vertices()[c].position;
            Vec3 pd = _mesh.vertices()[d].position;

            // a、b 在第 i - 1 圈，c、d 在第 i 圈；從外側看 a 左下、b 右下、c 左上、d 右上。
            // 跳過面積為 0 的格子：折角處同一位置的兩個剖面點；上下兩圈都在軸上；
            // 以及不平滑時相鄰兩面之間的空隙，那裡左右兩邊的頂點兩兩重合
            if ((pa == pb && pc == pd) || pa == pc) {
                continue;
            }

            // 一邊在軸上時格子退化成三角形，只留下有面積的那一個
            if (pa == pb) {
                _mesh.addTriangle(a, d, c);
            } else if (pc == pd) {
                _mesh.addTriangle(a, b, c);
            } else {
                _mesh.addTriangle(a, b, c);
                _mesh.addTriangle(b, d, c);
            }
        }
    }
}

#pragma endregion  // Lathe Implementation

}  // namespace robot::geometry
