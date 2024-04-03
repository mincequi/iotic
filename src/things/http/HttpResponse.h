#pragma once

#include <list>
#include <memory>
#include <string>

namespace uvw_net {

struct HttpResponse {
    std::list<std::pair<std::string, std::string>> headers;
    int statusCode = 0;
    std::string description;
    std::string body;

    bool messageComplete = false;
};

} // namespace uvw_net
