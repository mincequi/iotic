#include "HttpClient.h"

#include <uv/errno.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpSession.h"

#include <common/Logger.h>

namespace uvw_net {

HttpClient::HttpClient() {
}

HttpClientPtr HttpClient::create() {
    return std::shared_ptr<HttpClient>(new HttpClient);
}

void HttpClient::get(const HttpRequest& request, bool isFireAndForget) {
    if (_isRequestPending) {
        if (onErrorCallback) {
            onErrorCallback(uvw::error_event{(int)UV_ETIMEDOUT});
        }
    }
    _p = std::make_unique<HttpSession>(this);
    _p->request = request;
    _p->resolver->addr_info(request.host, std::to_string(request.port));
    _isRequestPending = !isFireAndForget;
}

} // namespace uvw_net
