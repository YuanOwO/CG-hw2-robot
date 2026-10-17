#include "graphics/3d/primitives.hpp"

#include "graphics/3d/shading.hpp"
#include "platform/glut.hpp"

namespace robot::graphics {

void drawAxes(float length) {
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glLineWidth(3.0f);

    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();

    glPopAttrib();
}

void drawLines(const std::vector<geometry::LineSegment3D>& segments, const ColorRGBA& color, float width) {
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glLineWidth(width);
    glColor4f(color.r, color.g, color.b, color.a);

    glBegin(GL_LINES);
    for (const auto& segment : segments) {
        const Vec3 start = segment.startPoint();
        const Vec3 end = segment.endPoint();
        glVertex3f(start.x, start.y, start.z);
        glVertex3f(end.x, end.y, end.z);
    }
    glEnd();

    glPopAttrib();
}

void drawPoints(const std::vector<Vec3>& points, const ColorRGBA& color, float size) {
    glPushAttrib(GL_POINT_BIT | GL_CURRENT_BIT);
    glPointSize(size);
    glColor4f(color.r, color.g, color.b, color.a);

    glBegin(GL_POINTS);
    for (const Vec3& point : points) {
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();

    glPopAttrib();
}

void drawQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const ColorRGBA& color) {
    auto triangle = [&color](const Vec3& p, const Vec3& q, const Vec3& r) {
        const Vec3 normal = normalize(cross(q - p, r - p));
        const float shade = shadeOf(normal);
        glColor4f(color.r * shade, color.g * shade, color.b * shade, color.a);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(p.x, p.y, p.z);
        glVertex3f(q.x, q.y, q.z);
        glVertex3f(r.x, r.y, r.z);
    };

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_TRIANGLES);
    triangle(a, b, c);
    triangle(a, c, d);
    glEnd();
    glPopAttrib();
}

}  // namespace robot::graphics
