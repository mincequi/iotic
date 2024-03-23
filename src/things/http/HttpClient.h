#pragma once

#include <map>
#include <memory>
#include <string>

#include <uvw.hpp>

namespace uvw_net {

class HttpSession;
class HttpRequest;
struct HttpResponse;

//using HttpResponsePtr = std::shared_ptr<HttpResponse>;
using OnResponseCallback = std::function<void(const HttpResponse&)>;
using OnErrorCallback = std::function<void(const uvw::error_event&)>;

class HttpClient;
using HttpClientPtr = std::shared_ptr<HttpClient>;

class HttpClient : public std::enable_shared_from_this<HttpClient> {
public:
    static HttpClientPtr create();

    void get(const HttpRequest& request, bool isFireAndForget = false);

    OnResponseCallback onResponseCallback = nullptr;
    OnErrorCallback onErrorCallback = nullptr;

private:
    HttpClient();
    void fetch(const sockaddr& addr,
               const OnResponseCallback& onResponseCallback,
               const OnErrorCallback& onErrorCallback);

    void writeRequest(uvw::tcp_handle& client);

    bool _isRequestPending = false;

    std::unique_ptr<HttpSession> _p;

    friend class uvw_net::HttpSession;
};

} // namespace uvw_net
