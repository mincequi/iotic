#pragma once

#include <things/Thing.h>

class QHttpServer;
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

    // TODO: Qt does not seem to work with a single QHttpServer serving HTTP and WS.
    //       It also does not work, when creating two instances of QHttpServer
    //       with one serving HTTP and the other serving WS.
    //       However it works, if a single instance serves HTTP OR WS!
    //       Also works if QHttpServer serves HTTP and QWebSocketServer serves WS!
    QHttpServer* _httpServer;
    std::unique_ptr<cmrc::embedded_filesystem> _fs;

    std::unique_ptr<uWS::App> _uwsApp;
};
