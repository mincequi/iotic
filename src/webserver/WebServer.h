#pragma once

#include <memory>

#include <MoveOnlyFunction.h>

class Config;
class WebAppRouter;

namespace cmrc {
class embedded_filesystem;
}

namespace uWS {

struct HttpRequest;
template <bool SSL>
struct HttpResponse;

template <bool SSL>
struct TemplatedApp;
typedef TemplatedApp<false> App;
}

namespace uvw_iot {
class ThingRepository;
namespace util {
class Site;
}
}

using uvw_iot::ThingRepository;
using uvw_iot::util::Site;

class WebServer {
public:
    explicit WebServer(const ThingRepository& repo, const Site& site, const Config& cfg);
    ~WebServer();

    void registerGetRoute(const std::string& path, uWS::MoveOnlyFunction<void(uWS::HttpResponse<false> *, uWS::HttpRequest *)> &&handler);

private:
    const ThingRepository& _repo;
    const Site& _site;
    const Config& _cfg;

    std::unique_ptr<cmrc::embedded_filesystem> _fs;
    std::unique_ptr<uWS::App> _uwsApp;
    std::shared_ptr<WebAppRouter> _webAppRouter;
};
