#include "graphics/primitives.hpp"

#include <cmath>

#include "math/constants.hpp"
#include "platform/glut.hpp"

namespace robot::graphics {

void drawAxes(float length) {
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glLineWidth(3.0f);

    glBegin(GL_LINES);

    // x 軸：紅色
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // y 軸：綠色
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // z 軸：藍色
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();

    glPopAttrib();
}

}  // namespace robot::graphics
