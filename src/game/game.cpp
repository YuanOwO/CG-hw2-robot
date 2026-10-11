#include "game/game.hpp"

#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include "common/color.hpp"
#include "font/font.hpp"
#include "game/screens/pause_screen.hpp"
#include "game/screens/title_screen.hpp"
#include "render/canvas.hpp"
#include "render/stage.hpp"

namespace robot::game {

namespace {

const FontStyle HUD_FONT = GlyphFontStyle{GlyphFont::UNIFONT, 16};

}  // namespace

Game::Game(engine::Engine& engine) : _engine(engine) {
    setScreen(std::make_unique<TitleScreen>(*this));
    applyPendingChanges();
}

Game::~Game() = default;

void Game::startWorld() {
    _pendingWorld = WorldChange::Start;
    setScreen(nullptr);
}

void Game::quitToTitle() {
    _pendingWorld = WorldChange::Unload;
    setScreen(std::make_unique<TitleScreen>(*this));
}

void Game::quit() {
    _engine.quit();
}

void Game::setScreen(std::unique_ptr<Screen> screen) {
    _pendingScreen = ScreenChange{std::move(screen), false};
}

void Game::openChildScreen(std::unique_ptr<Screen> screen) {
    _pendingScreen = ScreenChange{std::move(screen), true};
}

void Game::applyPendingChanges() {
    switch (_pendingWorld) {
    case WorldChange::Start:
        _world = std::make_unique<World>();
        break;
    case WorldChange::Unload:
        _world.reset();
        break;
    case WorldChange::None:
        break;
    }
    _pendingWorld = WorldChange::None;

    if (!_pendingScreen) {
        return;
    }

    ScreenChange change = std::move(*_pendingScreen);
    _pendingScreen.reset();

    if (change.asChild && change.screen) {
        change.screen->setParent(std::move(_screen));
    }
    _screen = std::move(change.screen);  // 沒有交給新畫面保管的舊畫面在這裡銷毀

    if (_screen) {
        _screen->init(_engine.width(), _engine.height());
        _screen->mouseMove(_engine.input().mouse().position());
    }
}

void Game::tick(float dt, const engine::Input& input) {
    if (_world && !_screen) {
        _world->tick(dt, input);
    }
    applyPendingChanges();
}

void Game::render() {
    if (_world) {
        _world->render(_engine.width(), _engine.height());
    }

    render::begin2D(_engine.width(), _engine.height());
    render::Canvas canvas;

    if (_world) {
        renderHud(canvas);
    }
    if (_screen) {
        _screen->render(canvas);
    }
}

void Game::onResize(int width, int height) {
    if (_screen) {
        _screen->init(width, height);
        _screen->mouseMove(_engine.input().mouse().position());
    }
}

void Game::onKeyDown(Key key, bool repeat) {
    if (_screen) {
        _screen->keyDown(key, repeat);
    } else if (_world && key == Key::Escape && !repeat) {
        setScreen(std::make_unique<PauseScreen>(*this));
    }
    applyPendingChanges();
}

void Game::onMouseDown(MouseButton button, Point position) {
    if (_screen) {
        _screen->mouseDown(button, position);
    }
    applyPendingChanges();
}

void Game::onMouseUp(MouseButton button, Point position) {
    if (_screen) {
        _screen->mouseUp(button, position);
    }
    applyPendingChanges();
}

void Game::onMouseMove(Point position) {
    if (_screen) {
        _screen->mouseMove(position);
    }
}

void Game::renderHud(render::Canvas& canvas) {
    const std::string text =
        "FPS " + std::to_string(_engine.framesPerSecond()) + " · TPS " + std::to_string(_engine.ticksPerSecond());
    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    canvas.fillRect(8, 8, textWidth + 12, textHeight + 8, ColorRGBA{0.0f, 0.0f, 0.0f, 0.6f});
    canvas.drawText(text, HUD_FONT, Color::White, Point(14.0f, 12.0f));
}

}  // namespace robot::game
