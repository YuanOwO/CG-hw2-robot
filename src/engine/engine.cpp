#include "engine/engine.hpp"

#include <algorithm>
#include <stdexcept>

#include "input/glut_keymap.hpp"
#include "platform/glut.hpp"

namespace robot::engine {

namespace {

// GLUT 的計時器 callback 只能帶一個 int，因此以全域指標找回 Engine。
Engine* currentEngine = nullptr;

}  // namespace

Engine::Engine(int& argc, char** argv, const std::string& title, int width, int height)
    : _width(width),
      _height(height),
      _assetsPath(std::filesystem::weakly_canonical(std::filesystem::absolute(argv[0])).parent_path() / "assets") {
    if (currentEngine != nullptr) {
        throw std::runtime_error("Only one Engine instance is allowed.");
    }

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
    _framesPerSecond = _frameCount / seconds;
    _ticksPerSecond = _tickCount / seconds;

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
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);  // 天空藍背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_app != nullptr) {
        _app->render();
    }

    glutSwapBuffers();
    _frameCount++;
}

#pragma endregion  // Window callbacks

#pragma region Keyboard callbacks

void Engine::onKeyboard(unsigned char key, int x, int y) {
    const Key mapped = fromGlutKeyboard(key, _input.keyboard().isCtrlDown());
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
    const Key mapped = fromGlutKeyboard(key, _input.keyboard().isCtrlDown());
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
    const Key mapped = fromGlutSpecial(key);
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
    const Key mapped = fromGlutSpecial(key);
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
    const MouseButton mapped = fromGlutMouseButton(button);
    if (mapped == MouseButton::Unknown) {
        return;
    }

    _input.moveMouse(x, y);
    const Point position(x, y);

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
        _app->onMouseMove(Point(x, y));
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
