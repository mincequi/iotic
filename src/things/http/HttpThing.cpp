#include "HttpThing.h"

#include <common/Logger.h>
#include <things/ThingsRepository.h>

#include "client.h"
#include "common.h"

using namespace std::placeholders;

struct HttpThing::Impl {
    http::client client;
};

HttpThing::HttpThing(const ThingInfo& info)
    : Thing{info},
      _p(std::make_unique<Impl>()) {
}

HttpThing::~HttpThing() {
}

void HttpThing::read(const std::string& url) {
    http::request req;
    req.url = "http://" + url;
    //_p->client.fetch(req, std::bind(&HttpThing::onHttpRead, this, _1, _2));
    _p->client.fetch(req, std::bind(&ThingsRepository::onRead, repo, id(), _1, _2));
}

void HttpThing::write(const std::string& url) {
    http::request req;
    req.url = "http://" + url;
    _p->client.fetch(req, std::bind(&ThingsRepository::onWrite, repo, id(), _2));
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
