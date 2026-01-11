#pragma once

#include <uvw/emitter.h>
#include <uvw/loop.h>
#include <uvw/udp.h>

#include <coap/CoapMessage.h>

class CoapDiscovery final: public uvw::emitter<CoapDiscovery, CoapMessage> {
public:
    CoapDiscovery();

    void discover();

private:
    std::shared_ptr<uvw::udp_handle> _udp = uvw::loop::get_default()->resource<uvw::udp_handle>();
};
