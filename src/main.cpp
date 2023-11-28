#include <QCoreApplication>

#include <uvw/loop.h>

#include "appDaemon/AppBackend.h"
#include "appDiscover/AppDiscover.h"

#include <cli/Cli.h>
#include <common/Logger.h>

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    std::unique_ptr<AppBackend> backend;
    std::unique_ptr<AppDiscover> discover;

    // Parse command line
    switch (Cli::parseCommandLine(argc, argv)) {
    case mode::daemon:
        // Setup logger
        Logger::init(argc, argv);
        // Setup app backend
        backend = std::make_unique<AppBackend>();
        break;
    case mode::discover:
        // Setup logger
        Logger::init(argc, argv);
        // Setup app backend
        discover = std::make_unique<AppDiscover>();
        break;
    case mode::help:
    return 0;
    }

    // Setup main loop (drive libuv event loop from Qt using QTimer)
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, []() {
        uvw::loop::get_default()->run(uvw::loop::run_mode::NOWAIT);
    });
    timer.start(100ms);

    return a.exec();
}
