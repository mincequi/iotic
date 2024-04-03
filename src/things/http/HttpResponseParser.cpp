#include "HttpResponseParser.h"

namespace uvw_net {

HttpResponseParser::HttpResponseParser() {
    llhttp_settings_init(&_settings);
    _settings.on_body = onBody;
    _settings.on_header_field = onHeaderField;
    _settings.on_header_value = onHeaderValue;
    _settings.on_message_complete = onMessageComplete;
    llhttp_init(&_parser, HTTP_RESPONSE, &_settings);
    _parser.data = this;
}

std::optional<HttpResponse> HttpResponseParser::parse(const char* data, size_t size) {
    auto err = llhttp_execute(&_parser, data, size);
    if (err != HPE_OK || !_isComplete) {
        return {};
    }
    return _response;
}

int HttpResponseParser::onBody(llhttp_t* self, const char* at, size_t length) {
    ((HttpResponseParser*)self->data)->_response.body = std::string(at, length);
    return 0;
}

int HttpResponseParser::onHeaderField(llhttp_t* self, const char* at, size_t length) {
    auto body = std::string(at, length);
    ((HttpResponseParser*)self->data)->_response.headers.push_back({std::string(at, length), ""});
    return 0;
}

int HttpResponseParser::onHeaderValue(llhttp_t* self, const char* at, size_t length) {
    auto body = std::string(at, length);
    ((HttpResponseParser*)self->data)->_response.headers.back().second.assign(at, length);
    return 0;
}

int HttpResponseParser::onMessageComplete(llhttp_t* self) {
    ((HttpResponseParser*)self->data)->_isComplete = true;
    return 0;
}

} // namespace uvw_net
