#include "HttpThing.h"

#include <common/Logger.h>
#include <things/ThingsRepository.h>

#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std::placeholders;

struct HttpThing::Impl {
    Impl(HttpThing* q) : _q(q) {
        client->onErrorCallback = [this](const uvw::error_event& error) {
            LOG_S(WARNING) << _q->id() << "> " << error.what();
            repo->onError(_q->id(), error.code());
        };
        client->onResponseCallback = [this](uvw_net::HttpResponse response) {
            for (const auto& h : response.headers) {
                DLOG_S(1) << h.first << ": " << h.second;
            }
            if (!response.body.empty()) {
                DLOG_S(1) << _q->id() << "> body: " << response.body;
                repo->onRead(_q->id(), response.body);
            }
        };
    }

    HttpThing* _q;
    uvw_net::HttpClientPtr client = uvw_net::HttpClient::create();
};

HttpThing::HttpThing(const ThingInfo& info)
    : Thing{info},
      _p(std::make_unique<Impl>(this)) {
}

HttpThing::~HttpThing() {
}

void HttpThing::read(const std::string& host, const std::string& path) {
    uvw_net::HttpRequest request;
    request.host = host;
    request.path = path;
    _p->client->get(request);
}

void HttpThing::write(const std::string& host, const std::string& path) {
    uvw_net::HttpRequest request;
    request.host = host;
    request.path = path;
    _p->client->get(request, true);
}

void HttpThing::onHttpRead(const std::string& response, int error) {
    if (error < 0) {
        LOG_S(WARNING) << id() << "> read error: " << error;
        ++_errorCount;
        // We must not directly delete this thing because thing itself might still process something.
        if (_errorCount > 4) {
            stateSubscriber().on_next(State::Failed);
        }
        return;
    }

    _errorCount = 0;
    onRead(response);
}
