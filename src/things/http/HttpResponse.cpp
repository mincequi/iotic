#include "HttpResponse.h"

#include <llhttp.h>

#include <common/Logger.h>

namespace uvw_net {

class HttpParser {
public:
    static HttpParser* instance() {
        if (!_instance) _instance = new HttpParser();
        return _instance;
    }

    const HttpResponse& parse(const std::string& buffer) {
        //LOG_S(INFO) << "buffer: " << buffer;
        llhttp_reset(&_parser);
        _response = HttpResponse();
        auto err = llhttp_execute(&_parser, buffer.data(), buffer.length());
        if (err != HPE_OK) {
            // TODO
        }
        return _response;
    }

private:
    HttpParser() {
        llhttp_settings_init(&_settings);
        _settings.on_body = onBody;
        _settings.on_header_field = onHeaderField;
        _settings.on_header_value = onHeaderValue;
        llhttp_init(&_parser, HTTP_RESPONSE, &_settings);
        _parser.data = this;
    }

    static int onBody(llhttp_t* self, const char* at, size_t length) {
        ((HttpParser*)self->data)->_response.body = std::string(at, length);
        return 0;
    }

    static int onHeaderField(llhttp_t* self, const char* at, size_t length) {
        auto body = std::string(at, length);
        ((HttpParser*)self->data)->_response.headers.push_back({std::string(at, length), ""});
        return 0;
    }

    static int onHeaderValue(llhttp_t* self, const char* at, size_t length) {
        auto body = std::string(at, length);
        ((HttpParser*)self->data)->_response.headers.back().second.assign(at, length);
        return 0;
    }

    static inline HttpParser* _instance = nullptr;

    llhttp_t _parser;
    llhttp_settings_t _settings;
    HttpResponse _response;
};

const HttpResponse& HttpResponse::fromBuffer(const std::unique_ptr<char[]>& buffer, size_t length) {
    return HttpParser::instance()->parse(std::string(buffer.get(), length));
}

} // namespace uvw_net
