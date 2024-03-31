#pragma once

#include <optional>
#include <string>

#include <uvw_net/dns_sd/DnsLabels.h>

namespace uvw_net {

struct DnsRecordDataSrv {
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    std::string target;

    static std::optional<DnsRecordDataSrv> fromBuffer(const uint8_t* head, /*const DnsLabels& labels,*/ const uint8_t* data, size_t size);
};

} // namespace uvw_net
