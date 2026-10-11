#include "engine/engine.hpp"

#include <algorithm>
#include <cstdio>
#include <stdexcept>

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
        _app->tick(TICK_SECONDS);
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
    std::printf("FPS %.0f · TPS %.0f\n", _frameCount / seconds, _tickCount / seconds);
    std::fflush(stdout);

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

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-53.33, 53.33, -40.0, 40.0, 0.0, 120.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Engine::onVisibility(int state) {
    if (state == GLUT_VISIBLE) {
        _displayedFrame = -1;  // 重新顯示時強制重畫
    }
}

void Engine::onDisplay() {
    // 這一幀已經畫過：不重畫也不交換緩衝區，畫面維持剛才的內容
    if (_displayedFrame == _frameIndex) {
        return;
    }
    _displayedFrame = _frameIndex;

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

}  // namespace robot::engine
