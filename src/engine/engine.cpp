#include "engine/engine.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include "io/font_catalog.hpp"
#include "platform/glut.hpp"

namespace robot::engine {

namespace {

// GLUT 的計時器 callback 只能帶一個 int，因此以全域指標找回 Engine。
Engine* currentEngine = nullptr;

}  // namespace

Engine::Engine(int& argc, char** argv, const std::string& title, int width, int height)
    : _width(width), _height(height) {
    if (currentEngine != nullptr) {
        throw std::runtime_error("Only one Engine instance is allowed.");
    }

    // 載入失敗會拋出例外，因此在設定 currentEngine 之前執行
    const auto executablePath = std::filesystem::weakly_canonical(std::filesystem::absolute(argv[0]));
    io::registerFonts(executablePath.parent_path() / "assets" / "fonts" / "fonts.ini");

    currentEngine = this;

    platform::initialize(argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    _windowId = platform::createWindow(title.c_str(), *this);
}

Engine::~Engine() {
    // 主迴圈結束後視窗已由平台層銷毀
    if (_windowId != 0 && platform::isRunning()) {
        platform::destroyWindow(_windowId);
    }
    currentEngine = nullptr;
}

void Engine::run(Application& app) {
    _app = &app;
    _lastFrame = Clock::now();
    _statsStart = _lastFrame;

    glutTimerFunc(FRAME_INTERVAL_MS, timerCallback, 0);
    platform::runMainLoop();

    _app = nullptr;
}

#pragma region Main loop

void Engine::timerCallback(int) {
    // 視窗已關閉時不再續約，主迴圈隨後結束
    if (currentEngine == nullptr || currentEngine->_windowId == 0) {
        return;
    }
    currentEngine->frame();
}

void Engine::frame() {
    if (_quitRequested) {
        platform::destroyWindow(_windowId);
        _windowId = 0;
        return;
    }

    const auto now = Clock::now();
    const Clock::duration elapsed = now - _lastFrame;
    _lastFrame = now;

    _accumulator += std::min<Clock::duration>(elapsed, MAX_FRAME_DELTA);

    while (_accumulator >= TICK_DURATION) {
        _input.beginTick();
        _app->tick(TICK_SECONDS, _input);
        _input.endTick();

        _accumulator -= TICK_DURATION;
        _tickCount++;
    }

    updateStats(now);

    _frameIndex++;
    glutPostWindowRedisplay(_windowId);

    glutTimerFunc(FRAME_INTERVAL_MS, timerCallback, 0);
}

void Engine::updateStats(Clock::time_point now) {
    const auto elapsed = now - _statsStart;
    if (elapsed < std::chrono::seconds(1)) {
        return;
    }

    const double seconds = std::chrono::duration<double>(elapsed).count();
    _framesPerSecond = static_cast<int>(_frameCount / seconds + 0.5);
    _ticksPerSecond = static_cast<int>(_tickCount / seconds + 0.5);

    _frameCount = 0;
    _tickCount = 0;
    _statsStart = now;
}

#pragma endregion  // Main loop

#pragma region Window callbacks

void Engine::onClose() {
    // 平台層會在呼叫後銷毀視窗
    _windowId = 0;
}

void Engine::onReshape(int width, int height) {
    _width = width;
    _height = height;
    _displayedFrame = -1;

    if (_app != nullptr) {
        _app->onResize(width, height);
    }
}

void Engine::onVisibility(int state) {
    if (state == GLUT_VISIBLE) {
        _displayedFrame = -1;
    } else if (state == GLUT_NOT_VISIBLE) {
        _input.clear();
    }
}

void Engine::onDisplay() {
    if (_displayedFrame == _frameIndex) {
        return;
    }
    _displayedFrame = _frameIndex;

    glViewport(0, 0, _width, _height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_app != nullptr) {
        _app->render();
    }

    glutSwapBuffers();
    _frameCount++;
}

#pragma endregion  // Window callbacks

#pragma region Keyboard callbacks

Key Engine::mapKeyboardKey(unsigned char key) const {
    // 按住 Ctrl 時 GLUT 送出控制字元（Ctrl+A 為 1）
    return _input.keyboard().isCtrlDown() ? mapCharacterWithCtrl(key) : mapCharacter(key);
}

void Engine::onKeyboard(unsigned char key, int x, int y) {
    const Key mapped = mapKeyboardKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    const bool firstPress = _input.keyDown(mapped);

    if (_app != nullptr) {
        _app->onKeyDown(mapped, !firstPress);
    }
}

void Engine::onKeyboardUp(unsigned char key, int x, int y) {
    const Key mapped = mapKeyboardKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyUp(mapped);

    if (_app != nullptr) {
        _app->onKeyUp(mapped);
    }
}

void Engine::onSpecial(int key, int x, int y) {
    const Key mapped = mapSpecialKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    const bool firstPress = _input.keyDown(mapped);

    if (_app != nullptr) {
        _app->onKeyDown(mapped, !firstPress);
    }
}

void Engine::onSpecialUp(int key, int x, int y) {
    const Key mapped = mapSpecialKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyUp(mapped);

    if (_app != nullptr) {
        _app->onKeyUp(mapped);
    }
}

#pragma endregion  // Keyboard callbacks

#pragma region Mouse callbacks

void Engine::onMouse(int button, int state, int x, int y) {
    const MouseButton mapped = mapMouseButton(button);
    if (mapped == MouseButton::Unknown) {
        return;
    }

    _input.moveMouse(x, y);
    const Point position(static_cast<float>(x), static_cast<float>(y));

    if (state == GLUT_DOWN) {
        _input.mouseDown(mapped);
        if (_app != nullptr) {
            _app->onMouseDown(mapped, position);
        }
    } else if (state == GLUT_UP) {
        _input.mouseUp(mapped);
        if (_app != nullptr) {
            _app->onMouseUp(mapped, position);
        }
    }
}

void Engine::onMotion(int x, int y) {
    _input.moveMouse(x, y);
    if (_app != nullptr) {
        _app->onMouseMove(Point(static_cast<float>(x), static_cast<float>(y)));
    }
}

void Engine::onPassiveMotion(int x, int y) {
    onMotion(x, y);
}

void Engine::onEntry(int state) {
    _input.clearMouseButtons();  // 在視窗外放開時收不到事件
}

#pragma endregion  // Mouse callbacks

}  // namespace robot::engine
