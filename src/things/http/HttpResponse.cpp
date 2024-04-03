#include "HttpResponse.h"

#include <llhttp.h>

#include <common/Logger.h>

namespace uvw_net {

class HttpResponseParser {
public:
    HttpResponseParser() {
        llhttp_settings_init(&_settings);
        _settings.on_body = onBody;
        _settings.on_header_field = onHeaderField;
        _settings.on_header_value = onHeaderValue;
        llhttp_init(&_parser, HTTP_RESPONSE, &_settings);
        _parser.data = this;
    }

    std::optional<HttpResponse> parse(const char* data, size_t size) {
        auto err = llhttp_execute(&_parser, data, size);
        if (err != HPE_OK) {
            return {};
        }
        return _response;
    }

private:
    static int onBody(llhttp_t* self, const char* at, size_t length) {
        ((HttpResponseParser*)self->data)->_response.body = std::string(at, length);
        return 0;
    }

    static int onHeaderField(llhttp_t* self, const char* at, size_t length) {
        auto body = std::string(at, length);
        ((HttpResponseParser*)self->data)->_response.headers.push_back({std::string(at, length), ""});
        return 0;
    }

    static int onHeaderValue(llhttp_t* self, const char* at, size_t length) {
        auto body = std::string(at, length);
        ((HttpResponseParser*)self->data)->_response.headers.back().second.assign(at, length);
        return 0;
    }

    llhttp_t _parser;
    llhttp_settings_t _settings;
    HttpResponse _response;
};

} // namespace uvw_net
