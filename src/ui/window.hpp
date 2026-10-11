#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

#include "common/point.hpp"
#include "event/events.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"
#include "platform/platform.hpp"
#include "render/renderer.hpp"
#include "ui/event_target.hpp"

namespace robot::ui {

const int FRAME_RATE = 60;
const int FRAME_INTERVAL_MS = 1000 / FRAME_RATE;
const float CLICK_MOVE_THRESHOLD = 4.0f;
const std::chrono::milliseconds DOUBLE_CLICK_TIME_THRESHOLD(300);

// 單次 update 允許推進的最長時間。主迴圈停頓（拖曳視窗、除錯中斷）後，
// 動畫只前進這麼多，而不是一次補上整段停頓。
const std::chrono::milliseconds MAX_FRAME_DELTA(100);

class Element;
class RootElement;

class Window : public EventTarget, private platform::WindowHandler {
   public:
    Window(const std::string& title, int width, int height, bool resizable = true);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Properties

    int id() const { return _id; }

    int width() const { return _width; }
    int height() const { return _height; }

    const std::string& title() const { return _title; }

    bool isResizable() const { return _resizable; }

    bool shouldClose() const { return _shouldClose; }

    // Modal windows exclusively receive input until they are closed.
    static bool canReceiveInput(int windowId);
    static void activateModalWindow();

    // Content

    void setContent(std::unique_ptr<Element> content);

   protected:
    friend class Element;

    // Properties

    void setTitle(const std::string& title);

    // 要求 GLUT 調整視窗大小；實際尺寸會在 reshape callback 中更新。
    void resize(int width, int height);

    void close();
    void setModal(bool modal);

    // Input state

    const KeyboardState& keyboardState() const { return _keyboardState; }

    const MouseState& mouseState() const { return _mouseState; }

    void resetInputState();
    void setFocusedElement(Element* element);

    // Geometry

    void requestLayout();
    void ensureLayout();

    // Rendering

    void requestRedisplay();

    // 每次 timer 觸發時呼叫（約 60 次 / 秒），在元素樹更新之前；delta 為實際經過的時間
    virtual void update(std::chrono::milliseconds delta) {}

   private:
    struct ClickCandidate {
        bool active = false;
        Point position;
    };

    struct ClickHistory {
        bool active = false;
        Point position;
        std::chrono::steady_clock::time_point time;
    };

    // Element interaction

    Element* hitTest(Point point);

    template <typename EventType, typename... Args>
    Element* dispatchMouseEvent(Args&&... args);

    void captureMouse(Element* element) { _mouseCapture = element; }

    void releaseMouseCapture() { _mouseCapture = nullptr; }

    void releaseMouseCaptureIf(Element* element) {
        if (_mouseCapture == element) {
            _mouseCapture = nullptr;
        }
    }

    void detachElementSubtree(Element* subtreeRoot);

    // Window properties

    int _id = 0;

    int _width;
    int _height;

    std::string _title;
    bool _resizable;

    bool _shouldClose = false;

    // Geometry

    void updateLayout();  // 更新佈局

    bool _layoutDirty = true;

    // Rendering

    render::Renderer _renderer;

    // 上一次 timer 觸發的時間，用來計算實際經過的時間
    std::chrono::steady_clock::time_point _lastUpdate = std::chrono::steady_clock::now();

    // Element tree

    std::unique_ptr<RootElement> _rootElement;

    // Interaction state

    Element* _focusedElement = nullptr;
    Element* _hoveredElement = nullptr;
    Element* _mouseCapture = nullptr;

    // Input state

    static KeyboardState _keyboardState;
    MouseState _mouseState;

    // Mouse click state

    std::unordered_map<MouseButton, ClickCandidate> _clickCandidate;
    std::unordered_map<MouseButton, ClickHistory> _lastClicks;

    // Internal event handlers

    void keyDownHandler(Key key, int x, int y);
    void keyUpHandler(Key key, int x, int y);
    void mouseMoveHandler(int x, int y);

    // GLUT timer callback

    static void timerCallback(int windowId);

    // platform::WindowHandler

    void onClose() final;
    void onReshape(int width, int height) final;
    void onVisibility(int state) final;
    void onDisplay() final;

    void onKeyboard(unsigned char key, int x, int y) final;
    void onKeyboardUp(unsigned char key, int x, int y) final;
    void onSpecial(int key, int x, int y) final;
    void onSpecialUp(int key, int x, int y) final;

    void onMouse(int button, int state, int x, int y) final;
    void onMotion(int x, int y) final;
    void onPassiveMotion(int x, int y) final;
    void onEntry(int state) final;
};

}  // namespace robot::ui
