#include "game/game.hpp"

#include <memory>
#include <utility>

#include "graphics/2d/canvas.hpp"
#include "graphics/render_mode.hpp"
#include "io/font_catalog.hpp"
#include "screen/pause_screen.hpp"
#include "screen/title_screen.hpp"

namespace robot::game {

Game::Game(engine::Engine& engine) : _engine(engine) {
    // 主選單建立按鈕時就要量文字寬度，字型必須先登記
    io::registerFonts(engine.assetsPath() / "fonts" / "fonts.ini");
    setScreen(std::make_unique<screen::TitleScreen>(*this));
    applyPendingRequests();  // 第一幀可能早於任何 tick 或事件，主選單必須先就位
}

Game::~Game() = default;

#pragma region screen::ScreenHost

void Game::startWorld() {
    _pendingSession = SessionRequest{std::make_unique<Session>()};
    setScreen(nullptr);
}

void Game::quitToTitle() {
    _pendingSession = SessionRequest{nullptr};
    setScreen(std::make_unique<screen::TitleScreen>(*this));
}

void Game::quit() {
    _engine.quit();
}

void Game::setScreen(std::unique_ptr<screen::Screen> screen) {
    _pendingScreen = ScreenRequest{std::move(screen), false};
}

void Game::openChildScreen(std::unique_ptr<screen::Screen> screen) {
    _pendingScreen = ScreenRequest{std::move(screen), true};
}

#pragma endregion  // screen::ScreenHost

#pragma region Pending requests

void Game::applyPendingRequests() {
    if (_pendingSession) {
        _session = std::move(_pendingSession->session);
        _pendingSession.reset();
    }

    if (!_pendingScreen) {
        return;
    }

    ScreenRequest request = std::move(*_pendingScreen);
    _pendingScreen.reset();

    if (request.asChild && request.screen) {
        request.screen->setParent(std::move(_screen));
    }
    _screen = std::move(request.screen);  // 沒有交給新畫面保管的舊畫面在這裡銷毀

    if (_screen) {
        if (_session) {
            _session->pause();
        }
        _screen->init(_engine.width(), _engine.height());
        // 游標不動就不會收到移動事件，新按鈕的懸停狀態要主動同步
        _screen->mouseMove(_engine.input().mouse().position());
    }
}

#pragma endregion  // Pending requests

#pragma region engine::Application

void Game::tick(float dt, const engine::Input& input) {
    // 有畫面時暫停：世界仍會繪製，只是不再前進
    if (_session && !_screen) {
        _session->tick(dt, input);
    }
    applyPendingRequests();
}

void Game::render() {
    if (_session) {
        _session->renderWorld(_engine.width(), _engine.height());
    }

    graphics::begin2D(_engine.width(), _engine.height());
    graphics::Canvas canvas;

    // HUD 只在遊玩時顯示：選單打開時世界已暫停，HUD 的資訊沒有意義，也會干擾選單
    if (_screen) {
        _screen->render(canvas);
    } else if (_session) {
        _session->renderHud(canvas, _engine.framesPerSecond(), _engine.ticksPerSecond());
    }
}

void Game::onResize(int width, int height) {
    // 按鈕位置依視窗大小計算，必須重建；重建後懸停狀態同樣要重新同步
    if (_screen) {
        _screen->init(width, height);
        _screen->mouseMove(_engine.input().mouse().position());
    }
}

void Game::onKeyDown(Key key, bool repeat) {
    // 有畫面時 Esc 由畫面處理（關閉畫面）；忽略自動重複，按住 Esc 關閉選單後才不會又立刻打開
    if (_screen) {
        _screen->keyDown(key, repeat);
    } else if (_session && key == Key::Escape && !repeat) {
        setScreen(std::make_unique<screen::PauseScreen>(*this));
    }
    applyPendingRequests();
}

void Game::onMouseDown(MouseButton button, Point position) {
    if (_screen) {
        _screen->mouseDown(button, position);
    }
    applyPendingRequests();
}

void Game::onMouseUp(MouseButton button, Point position) {
    if (_screen) {
        _screen->mouseUp(button, position);
    }
    applyPendingRequests();
}

void Game::onMouseMove(Point position) {
    // 只會更新懸停狀態，不會觸發按鈕，因此沒有請求需要套用
    if (_screen) {
        _screen->mouseMove(position);
    }
}

#pragma endregion  // engine::Application

}  // namespace robot::game
