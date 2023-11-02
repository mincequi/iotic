#pragma once

#include <things/Thing.h>

class ThingsRepository;

namespace cmrc {
class embedded_filesystem;
}

namespace uWS {
template <bool SSL>
struct TemplatedApp;
typedef TemplatedApp<false> App;
}

class WebServer {
public:
    explicit WebServer(void* mainLoop,
                       const ThingsRepository& thingsRepository);
    ~WebServer();

private:
    static std::string serializeUserProperties(const ThingPtr& thing);

    const ThingsRepository& _thingsRepository;
    std::unique_ptr<cmrc::embedded_filesystem> _fs;
    std::unique_ptr<uWS::App> _uwsApp;
};
