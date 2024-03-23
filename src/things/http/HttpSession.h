#pragma once

#include <memory>

#include <uvw/dns.h>
#include <uvw/loop.h>
#include <uvw/tcp.h>

#include <HttpRequest.h>

namespace uvw_net {

class HttpClient;

class HttpSession {
public:
    HttpSession(HttpClient* q);
    ~HttpSession();

private:
    std::shared_ptr<uvw::get_addr_info_req> resolver = uvw::loop::get_default()->resource<uvw::get_addr_info_req>();
    std::shared_ptr<uvw::tcp_handle> tcp = uvw::loop::get_default()->resource<uvw::tcp_handle>();

    HttpClient* _q;
    HttpRequest request;

    friend class HttpClient;
};

} // namespace uvw_net
