#include "CoapDiscovery.h"

#include <common/Logger.h>

CoapDiscovery::CoapDiscovery() {
    _udp->on<uvw::udp_data_event>([this](const uvw::udp_data_event& event, uvw::udp_handle& udp) {
        // For now, just drop partial packets
        if (event.partial) {
            LOG_S(WARNING) << "partial udp packet received, ignoring";
            return;
        }

        auto msg = CoapMessage::decode({event.data.get(), event.length}, event.sender.ip, event.sender.port);
        if (!msg) {
            LOG_S(WARNING) << "invalid coap message" << std::endl;
            return;
        }

        publish(*msg);
    });

    _udp->bind("0.0.0.0", 5683, uvw::udp_handle::udp_flags::REUSEADDR);
    uv_udp_set_membership(_udp->raw(), "224.0.1.187", NULL, static_cast<uv_membership>(uvw::udp_handle::membership::JOIN_GROUP));
    _udp->recv();
}

void CoapDiscovery::discover() {
    auto msg = CoapMessage::buildDiscoveryRequest();
    auto buffer = msg.encode();
    _udp->send("224.0.1.187", 5683, (char*)buffer.data(), buffer.size());
}
