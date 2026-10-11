#include "engine/engine.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include "font/font.hpp"
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

    // 字型放在執行檔旁的 assets/fonts；載入失敗會拋出例外，因此在設定 currentEngine 之前執行
    const auto executablePath = std::filesystem::weakly_canonical(std::filesystem::absolute(argv[0]));
    initializeFonts(executablePath.parent_path() / "assets" / "fonts");

    currentEngine = this;

    platform::initialize(argc, argv);

    // 雙緩衝避免動畫閃爍；深度緩衝供之後的 3D 場景做隱藏面消除
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    // 建立視窗，之後這個視窗的事件都會呼叫 on* 成員函數
    _windowId = platform::createWindow(title.c_str(), *this);
}

Engine::~Engine() {
    // 主迴圈結束前就被銷毀（例如建構後發生例外）時，才需要自行關閉視窗
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
    const auto now = Clock::now();
    const Clock::duration elapsed = now - _lastFrame;
    _lastFrame = now;

    // 停頓的時間直接丟棄：只累積不超過 MAX_FRAME_DELTA 的部分
    _accumulator += std::min<Clock::duration>(elapsed, MAX_FRAME_DELTA);

    // 每累積滿一個 tick 的時間就執行一次，剩下的留給下一幀
    while (_accumulator >= TICK_DURATION) {
        _input.beginTick();
        _app->tick(TICK_SECONDS, _input);
        _input.endTick();

        _accumulator -= TICK_DURATION;
        _tickCount++;
    }

    updateStats(now);

    // 遊戲畫面持續變化，每幀都重繪
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
    _displayedFrame = -1;  // 視窗大小改變，舊畫面不能用了
}

void Engine::onVisibility(int state) {
    if (state == GLUT_VISIBLE) {
        _displayedFrame = -1;  // 重新顯示時強制重畫
    } else if (state == GLUT_NOT_VISIBLE) {
        _input.clear();  // 隱藏時可能收不到放開事件，避免卡鍵
    }
}

void Engine::onDisplay() {
    // 這一幀已經畫過：不重畫也不交換緩衝區，畫面維持剛才的內容
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

    // 在後緩衝畫完後交換，顯示這一幀
    glutSwapBuffers();
    _frameCount++;
}

#pragma endregion  // Window callbacks

#pragma region Keyboard callbacks

Key Engine::mapKeyboardKey(unsigned char key) const {
    // 按住 Ctrl 時 GLUT 送出的是控制字元（例如 Ctrl+A 是 1），需要另外對應回字母鍵
    return _input.keyboard().isCtrlDown() ? mapCharacterWithCtrl(key) : mapCharacter(key);
}

void Engine::onKeyboard(unsigned char key, int x, int y) {
    const Key mapped = mapKeyboardKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyDown(mapped);
}

void Engine::onKeyboardUp(unsigned char key, int x, int y) {
    const Key mapped = mapKeyboardKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyUp(mapped);
}

void Engine::onSpecial(int key, int x, int y) {
    const Key mapped = mapSpecialKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyDown(mapped);
}

void Engine::onSpecialUp(int key, int x, int y) {
    const Key mapped = mapSpecialKey(key);
    if (mapped == Key::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    _input.keyUp(mapped);
}

#pragma endregion  // Keyboard callbacks

#pragma region Mouse callbacks

void Engine::onMouse(int button, int state, int x, int y) {
    const MouseButton mapped = mapMouseButton(button);
    if (mapped == MouseButton::Unknown) {
        return;
    }
    _input.moveMouse(x, y);
    if (state == GLUT_DOWN) {
        _input.mouseDown(mapped);
    } else if (state == GLUT_UP) {
        _input.mouseUp(mapped);
    }
}

void Engine::onMotion(int x, int y) {
    _input.moveMouse(x, y);
}

void Engine::onPassiveMotion(int x, int y) {
    _input.moveMouse(x, y);
}

void Engine::onEntry(int state) {
    _input.clearMouseButtons();  // 進入或離開視窗時，可能錯過放開事件
}

#pragma endregion  // Mouse callbacks

}  // namespace robot::engine
