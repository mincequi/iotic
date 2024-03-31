#pragma once

#include <uvw_net/dns_sd/DnsRecordClass.h>
#include <uvw_net/dns_sd/DnsRecordData.h>
#include <uvw_net/dns_sd/DnsRecordType.h>

namespace uvw_net {

// +--+--+--+--
// /   NAME   /
// +--+--+--+--
// |   TYPE   |
// +--+--+--+--
// |   CLASS  |
// +--+--+--+--
// |   TTL    |
// +--+--+--+--
// | RDLENGTH |
// +--+--+--+--
// /   RDATA  /
// +--+--+--+--

struct DnsAnswer {
	std::string name;
	DnsRecordType type = DnsRecordType::Invalid;
	DnsRecordClass cls = DnsRecordClass::Invalid;
	uint32_t ttl = 0;
	DnsRecordData data;

    const static uint16_t minSize = 6;
};

} // namespace uvw_net
