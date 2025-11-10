#include <uvw/loop.h>

#include "appDaemon/AppBackend.h"
#include "appDiscover/AppDiscover.h"
#include "appDiscoverCoro/AppDiscoverCoro.h"

#include <cli/Cli.h>
#include <common/Logger.h>

int main(int argc, char *argv[]) {
    std::unique_ptr<AppBackend> backend;
    std::unique_ptr<AppDiscover> discover;
    std::unique_ptr<AppDiscoverCoro> discoverCoro;

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
    case mode::discoverCoro:
        // Setup logger
        Logger::init(argc, argv);
        // Setup app backend
        discoverCoro = std::make_unique<AppDiscoverCoro>();
        break;
    case mode::help:
    return 0;
    }

    if (discoverCoro) {
        return uvw::loop::get_default()->run();
    }

    return uvw::loop::get_default()->run();
}
