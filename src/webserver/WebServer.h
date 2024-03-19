#pragma once

#include <things/Site.h>
#include <things/Thing.h>

class Config;
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
    explicit WebServer(const ThingsRepository& thingsRepository);
    ~WebServer();

private:
    const ThingsRepository& _thingsRepository;
    std::unique_ptr<cmrc::embedded_filesystem> _fs;
    std::unique_ptr<uWS::App> _uwsApp;
};
