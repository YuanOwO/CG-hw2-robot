#pragma once

#include <chrono>
#include <string>

#include "engine/application.hpp"
#include "engine/input.hpp"
#include "platform/platform.hpp"

namespace robot::engine {

const int TICK_RATE = 60;  // 每秒 tick 次數
const float TICK_SECONDS = 1.0f / TICK_RATE;
const std::chrono::nanoseconds TICK_DURATION(1'000'000'000 / TICK_RATE);

// 單幀最多推進的時間；主迴圈停頓後，超過的部分直接丟棄。
const std::chrono::milliseconds MAX_FRAME_DELTA(100);

const int FRAME_INTERVAL_MS = 16;  // 計時器間隔，約 60 FPS

// 遊戲引擎：建立並管理唯一的視窗，執行主迴圈。
class Engine : private platform::WindowHandler {
   public:
    Engine(int& argc, char** argv, const std::string& title, int width, int height);
    ~Engine() override;

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // 進入主迴圈，視窗關閉後返回。app 必須在返回前保持有效。
    void run(Application& app);

    // 在下一次計時器觸發時關閉視窗，run() 隨後返回。
    void quit() { _quitRequested = true; }

    int width() const { return _width; }
    int height() const { return _height; }

    // 最近一秒實際的繪製次數與 tick 次數
    double framesPerSecond() const { return _framesPerSecond; }
    double ticksPerSecond() const { return _ticksPerSecond; }

    const Input& input() const { return _input; }

   private:
    using Clock = std::chrono::steady_clock;

    int _windowId = 0;  // 0 表示視窗已關閉
    int _width;
    int _height;

    Application* _app = nullptr;
    Input _input;
    bool _quitRequested = false;

    Clock::time_point _lastFrame;
    Clock::duration _accumulator{0};  // 尚未用來執行 tick 的時間

    // Apple GLUT 每次要求重繪會呼叫兩次 display callback，同一幀只繪製一次。
    long long _frameIndex = 0;
    long long _displayedFrame = -1;  // -1：強制下一次 display 繪製

    // FPS / TPS 統計
    Clock::time_point _statsStart;
    int _frameCount = 0;
    int _tickCount = 0;
    double _framesPerSecond = 0.0;
    double _ticksPerSecond = 0.0;

    static void timerCallback(int value);
    void frame();
    void updateStats(Clock::time_point now);

    Key mapKeyboardKey(unsigned char key) const;

    // platform::WindowHandler

    void onClose() override;
    void onReshape(int width, int height) override;
    void onVisibility(int state) override;
    void onDisplay() override;

    void onKeyboard(unsigned char key, int x, int y) override;
    void onKeyboardUp(unsigned char key, int x, int y) override;
    void onSpecial(int key, int x, int y) override;
    void onSpecialUp(int key, int x, int y) override;

    void onMouse(int button, int state, int x, int y) override;
    void onMotion(int x, int y) override;
    void onPassiveMotion(int x, int y) override;
    void onEntry(int state) override;
};

}  // namespace robot::engine
