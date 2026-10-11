#include "app/application.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include "font/font.hpp"
#include "platform/glut.hpp"
#include "platform/platform.hpp"

namespace robot::app {

namespace {

Application* currentApp = nullptr;

}

bool Application::_instanceExists = false;

Application::Application(int& argc, char** argv) {
    if (_instanceExists) {
        throw std::runtime_error("Only one instance of Application is allowed.");
    }

    // 字型放在執行檔旁的 assets/fonts；載入失敗會拋出例外，因此在設定實例旗標之前執行
    const auto executablePath = std::filesystem::weakly_canonical(std::filesystem::absolute(argv[0]));
    initializeFonts(executablePath.parent_path() / "assets" / "fonts");

    _instanceExists = true;
    currentApp = this;

    // 初始化 GLUT
    platform::initialize(argc, argv);

    // 雙緩衝避免動畫閃爍；深度緩衝供 3D 場景做隱藏面消除
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutIdleFunc(idleCallback);
}

Application::~Application() {
    _instanceExists = false;
    currentApp = nullptr;
}

Application& Application::current() {
    if (!currentApp) {
        throw std::runtime_error("No current Application instance.");
    }
    return *currentApp;
}

void Application::run() {
    // 進入 GLUT 主循環，所有視窗關閉後返回
    platform::runMainLoop();
}

void Application::removeClosedWindows() {
    _windows.erase(
        std::remove_if(_windows.begin(), _windows.end(), [](const auto& window) { return window->shouldClose(); }),
        _windows.end());
}

void Application::idleCallback() {
    if (!_instanceExists) {
        return;  // 如果 Application 實例不存在，直接返回
    }

    currentApp->removeClosedWindows();
}

}  // namespace robot::app
