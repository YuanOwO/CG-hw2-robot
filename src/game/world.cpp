#include "game/world.hpp"

#include <cmath>

#include "platform/glut.hpp"
#include "render/stage.hpp"

namespace robot::game {

namespace {

const float ROTATION_SPEED = 45.0f;  // 度 / 秒：8 秒轉一圈

}  // namespace

void World::tick(float dt, const engine::Input& input) {
    _angle = std::fmod(_angle + ROTATION_SPEED * dt, 360.0f);
}

void World::render(int width, int height) const {
    render::begin3D(width, height, _camera, _projection);

    // 繞地板中心 (25, 0, 25) 旋轉：移到中心 → 旋轉 → 移回去（講義 01-2 p52）
    glTranslatef(25.0f, 0.0f, 25.0f);
    glRotatef(_angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-25.0f, 0.0f, -25.0f);

    // 從上方看為逆時針，正面朝上（講義 lecture 3 p24）
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 50.0f);
    glVertex3f(50.0f, 0.0f, 50.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);
    glEnd();
}

}  // namespace robot::game
