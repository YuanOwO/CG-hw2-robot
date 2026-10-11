#include "game/game.hpp"

#include <cmath>
#include <string>

#include "common/color.hpp"
#include "font/font.hpp"
#include "platform/glut.hpp"
#include "render/canvas.hpp"
#include "render/stage.hpp"

namespace robot::game {

namespace {

const float ROTATION_SPEED = 45.0f;  // 度 / 秒：8 秒轉一圈

const FontStyle HUD_FONT = GfntFontStyle{GfntFontId::UNIFONT_16};

}  // namespace

Game::Game(engine::Engine& engine) : _engine(engine) {}

void Game::tick(float dt, const engine::Input& input) {
    _angle = std::fmod(_angle + ROTATION_SPEED * dt, 360.0f);
}

void Game::render() {
    renderWorld();
    renderHud();  // 後畫的蓋在上面
}

void Game::renderWorld() {
    render::begin3D(_engine.width(), _engine.height(), _camera, _projection);

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

void Game::renderHud() {
    render::begin2D(_engine.width(), _engine.height());
    render::Canvas canvas;

    const std::string text =
        "FPS " + std::to_string(_engine.framesPerSecond()) + " · TPS " + std::to_string(_engine.ticksPerSecond());
    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    // 半透明黑底，讓文字在白色背景和黑色地板上都看得清楚
    canvas.fillRect(8, 8, textWidth + 12, textHeight + 8, ColorRGBA{0.0f, 0.0f, 0.0f, 0.6f});
    canvas.drawText(text, HUD_FONT, Color::White, Point(14.0f, 12.0f));
}

}  // namespace robot::game
