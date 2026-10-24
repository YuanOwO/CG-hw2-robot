#include "view/collider_renderer.hpp"

#include <vector>

#include "common/color.hpp"
#include "geometry/curve/line_segment.hpp"
#include "graphics/3d/primitives.hpp"
#include "platform/glut.hpp"

namespace robot::view {

void drawCollider(const world::collision::Box& collider) {
    const auto& min = -collider.halfExtents();
    const auto& max = collider.halfExtents();

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
    glTranslatef(collider.center().x, collider.center().y, collider.center().z);

    graphics::drawLines(edges, Color::White, 1.0f);

    glPopMatrix();
}

void drawCollider(const world::collision::UprightCylinder& collider) {
    const Vec3& baseCenter = collider.baseCenter();
    float radius = collider.radius();
    float height = collider.height();

    const int segments = 32;
    std::vector<geometry::LineSegment3D> edges;

    float lastX = radius * cos(2.0f * M_PI * (segments - 1) / segments);
    float lastZ = radius * sin(2.0f * M_PI * (segments - 1) / segments);

    for (int i = 0; i < segments; ++i) {
        float rad = 2.0f * M_PI * i / segments;

        float x = radius * cos(rad);
        float z = radius * sin(rad);

        Vec3 p1(lastX, baseCenter.y, lastZ);
        Vec3 p2(x, baseCenter.y, z);
        Vec3 p3(lastX, p1.y + height, lastZ);
        Vec3 p4(x, p2.y + height, z);

        edges.emplace_back(p1, p2);  // Bottom circle
        edges.emplace_back(p3, p4);  // Top circle
        if (i % 8 == 0) {
            edges.emplace_back(p2, p4);  // Vertical edge
        }

        lastX = x, lastZ = z;
    }

    glPushMatrix();
    glTranslatef(baseCenter.x, 0.0f, baseCenter.z);

    graphics::drawLines(edges, Color::White, 1.0f);

    glPopMatrix();
}

}  // namespace robot::view
