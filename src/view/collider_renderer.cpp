#include "view/collider_renderer.hpp"

#include <variant>
#include <vector>

#include "common/color.hpp"
#include "geometry/curve/line_segment.hpp"
#include "graphics/3d/primitives.hpp"
#include "math/constants.hpp"
#include "platform/glut.hpp"

namespace robot::view {

void drawCollider(const world::collision::UprightBox& collider) {
    const Vec3& halfSize = collider.size() * 0.5f;
    Vec3 min = Vec3(-halfSize.x, 0.0f, -halfSize.z);
    Vec3 max = Vec3(halfSize.x, collider.size().y, halfSize.z);

    std::vector<geometry::LineSegment3D> edges = {
        {Vec3(min.x, min.y, min.z), Vec3(max.x, min.y, min.z)},
        {Vec3(max.x, min.y, min.z), Vec3(max.x, min.y, max.z)},
        {Vec3(max.x, min.y, max.z), Vec3(min.x, min.y, max.z)},
        {Vec3(min.x, min.y, max.z), Vec3(min.x, min.y, min.z)},

        {Vec3(min.x, max.y, min.z), Vec3(max.x, max.y, min.z)},
        {Vec3(max.x, max.y, min.z), Vec3(max.x, max.y, max.z)},
        {Vec3(max.x, max.y, max.z), Vec3(min.x, max.y, max.z)},
        {Vec3(min.x, max.y, max.z), Vec3(min.x, max.y, min.z)},

        {Vec3(min.x, min.y, min.z), Vec3(min.x, max.y, min.z)},
        {Vec3(max.x, min.y, min.z), Vec3(max.x, max.y, min.z)},
        {Vec3(max.x, min.y, max.z), Vec3(max.x, max.y, max.z)},
        {Vec3(min.x, min.y, max.z), Vec3(min.x, max.y, max.z)}
    };

    glPushMatrix();
    glTranslatef(collider.baseCenter().x, collider.baseCenter().y, collider.baseCenter().z);
    glRotatef(collider.yaw(), 0.0f, 1.0f, 0.0f);

    graphics::drawLines(edges, Color::White, 1.0f);

    glPopMatrix();
}

void drawCollider(const world::collision::UprightCylinder& collider) {
    float radius = collider.radius();
    float height = collider.height();

    constexpr int segments = 32;
    std::vector<geometry::LineSegment3D> edges;

    float lastX = radius * cos(2.0f * PI * (segments - 1) / segments);
    float lastZ = radius * sin(2.0f * PI * (segments - 1) / segments);

    for (int i = 0; i < segments; ++i) {
        float rad = 2.0f * PI * i / segments;

        float x = radius * cos(rad);
        float z = radius * sin(rad);

        Vec3 p1(lastX, 0.0f, lastZ);
        Vec3 p2(x, 0.0f, z);
        Vec3 p3(lastX, height, lastZ);
        Vec3 p4(x, height, z);

        edges.emplace_back(p1, p2);  // Bottom circle
        edges.emplace_back(p3, p4);  // Top circle
        if (i % 8 == 0) {
            edges.emplace_back(p2, p4);  // Vertical edge
        }

        lastX = x, lastZ = z;
    }

    glPushMatrix();
    glTranslatef(collider.baseCenter().x, collider.baseCenter().y, collider.baseCenter().z);

    graphics::drawLines(edges, Color::White, 1.0f);

    glPopMatrix();
}

void drawCollider(const world::collision::Sphere& collider) {
    float radius = collider.radius();

    constexpr int segments = 32;
    constexpr int vRings = 8;
    constexpr int hRings = 3;
    std::vector<geometry::LineSegment3D> edges;

    for (int i = 0; i < vRings; i++) {
        float theta = 2.0f * PI * i / vRings;

        float x0 = radius * cos(theta);
        float z0 = radius * sin(theta);

        for (int j = 1; j <= segments; j++) {
            float phi1 = PI * (j - 1) / segments - 0.5f * PI;
            float phi2 = PI * j / segments - 0.5f * PI;

            float y1 = radius * sin(phi1);
            float x1 = x0 * cos(phi1);
            float z1 = z0 * cos(phi1);

            float y2 = radius * sin(phi2);
            float x2 = x0 * cos(phi2);
            float z2 = z0 * cos(phi2);

            edges.emplace_back(Vec3(x1, y1, z1), Vec3(x2, y2, z2));
        }
    }

    for (int i = 0; i < hRings; i++) {
        float phi = PI * (i + 1) / (hRings + 1) - 0.5f * PI;

        float y = radius * sin(phi);
        float r = radius * cos(phi);

        float lastX = r * cos(2.0f * PI * (segments - 1) / segments);
        float lastZ = r * sin(2.0f * PI * (segments - 1) / segments);

        for (int j = 0; j < segments; j++) {
            float theta = 2.0f * PI * j / segments;

            float x = r * cos(theta);
            float z = r * sin(theta);

            edges.emplace_back(Vec3(lastX, y, lastZ), Vec3(x, y, z));

            lastX = x, lastZ = z;
        }
    }

    glPushMatrix();
    glTranslatef(collider.center().x, collider.center().y, collider.center().z);
    graphics::drawLines(edges, Color::White, 1.0f);
    glPopMatrix();
}

void drawCollider(const world::collision::Collider& collider) {
    std::visit([](const auto& c) { drawCollider(c); }, collider);
}

}  // namespace robot::view
