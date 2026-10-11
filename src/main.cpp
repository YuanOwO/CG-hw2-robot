#include "app/application.hpp"
#include "app/windows/world.hpp"

using namespace robot;
using namespace robot::app;

int main(int argc, char** argv) {
    Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<WorldWindow>("My World", 800, 600);

    // 運行應用程序
    app.run();

    return 0;
}
