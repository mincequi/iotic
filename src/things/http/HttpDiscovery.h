#pragma once

#include <uvw/udp.h>

#include <things/ThingsDiscovery.h>

namespace QMdnsEngine {
class Service;
};

class HttpDiscovery : public ThingsDiscovery {
public:
    HttpDiscovery();

    void start(int msec) override;
    void stop() override;

private:
    std::shared_ptr<uvw::udp_handle> _udp = uvw::loop::get_default()->resource<uvw::udp_handle>();
};
