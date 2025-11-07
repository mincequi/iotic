#pragma once

#include <memory>

#include <MoveOnlyFunction.h>

class ConfigRepository;
class StrategyRepository;
class SymbolRepository;
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
    explicit WebServer(const ThingRepository& repo,
                       const Site& site,
                       const ConfigRepository& cfg,
                       const StrategyRepository& strategyRepository,
                       const SymbolRepository& symbolRepository);
    ~WebServer();

    void registerGetRoute(const std::string& path, uWS::MoveOnlyFunction<void(uWS::HttpResponse<false>*, uWS::HttpRequest*)>&& handler) const;

private:
    const ThingRepository& _repo;
    const Site& _site;
    const ConfigRepository& _configRepository;
    const StrategyRepository& _strategyRepository;
    const SymbolRepository& _symbolRepository;

    std::unique_ptr<cmrc::embedded_filesystem> _fs;
    std::unique_ptr<uWS::App> _uwsApp;
    std::shared_ptr<WebAppRouter> _webAppRouter;
};
