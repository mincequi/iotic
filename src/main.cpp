#include <QCoreApplication>

#include <uvw/loop.h>

#include "AppBackend.h"

#include <common/Logger.h>

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup app backend
    AppBackend backend;

    // Setup main loop (drive libuv event loop from Qt using QTimer)
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, []() {
        uvw::loop::get_default()->run(uvw::loop::run_mode::NOWAIT);
    });
    timer.start(100ms);

    return a.exec();
}
