#include "HttpSession.h"

#include "HttpClient.h"
#include "HttpResponse.h"

#include <common/Logger.h>

namespace uvw_net {

HttpSession::HttpSession(HttpClient* q) :
    _q(q) {
    resolver->on<uvw::error_event>([this](const uvw::error_event& error, const uvw::get_addr_info_req&) {
        if (_q->onErrorCallback)
            _q->onErrorCallback(error);
    });
    resolver->on<uvw::addr_info_event>([this](const uvw::addr_info_event& addrInfoEvent, const uvw::get_addr_info_req&) {
        sockaddr addr = *(addrInfoEvent.data)->ai_addr;
        tcp->connect(addr);
    });

    tcp->on<uvw::error_event>([this](const uvw::error_event& error, uvw::tcp_handle& tcp) {
        if (_q->onErrorCallback)
            _q->onErrorCallback(error);
    });
    tcp->on<uvw::connect_event>([this](const uvw::connect_event&, uvw::tcp_handle& tcp) {
        auto[buffer, length] = request.serialize();
        tcp.write(std::move(buffer), length);
    });
    // Start reading after writing
    tcp->on<uvw::write_event>([](const uvw::write_event&, uvw::tcp_handle& tcp) {
        tcp.read();
    });
    tcp->on<uvw::data_event>([this](const uvw::data_event& event, uvw::tcp_handle& tcp) {
        _q->_isRequestPending = false;
        if (_q->onResponseCallback) {
            auto response = _parser.parse(event.data.get(), event.length);
            if (response) _q->onResponseCallback(*response);
        }
    });
}

HttpSession::~HttpSession() {
    resolver->reset();
    resolver->cancel();
    tcp->reset();
    tcp->close();
}

} // namespace uvw_net
