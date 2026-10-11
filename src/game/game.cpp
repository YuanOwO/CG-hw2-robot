#include "game/game.hpp"

#include <cmath>
#include <memory>
#include <string>

#include "common/color.hpp"
#include "font/font.hpp"
#include "render/canvas.hpp"
#include "render/stage.hpp"

namespace robot::game {

namespace {

const FontStyle HUD_FONT = GfntFontStyle{GfntFontId::UNIFONT_16};

}  // namespace

Game::Game(engine::Engine& engine) : _engine(engine), _world(std::make_unique<World>()) {}

void Game::tick(float dt, const engine::Input& input) {
    if (_world) {
        _world->tick(dt, input);
    }
}

void Game::render() {
    if (_world) {
        _world->render(_engine.width(), _engine.height());
    }
    renderHud();  // 後畫的蓋在上面
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
