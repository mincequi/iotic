#pragma once

#include <map>
#include <string>

namespace uvw_net {

class HttpRequest {
public:
    HttpRequest();

    std::map<std::string, std::string> headers;
    std::string currentHeaderName;
    std::string currentHeaderValue;
    std::string path = "/";
    std::string body;
    std::string method = "GET";
    std::string host;
    int port = 80;
    bool messageComplete = false;

private:
    std::pair<std::unique_ptr<char[]>, size_t> serialize();

    std::string _buffer;

    friend class HttpSession;
};

} // namespace uvw_net
