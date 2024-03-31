#pragma once

#include <uvw_net/dns_sd/DnsRecordClass.h>
#include <uvw_net/dns_sd/DnsRecordType.h>

namespace uvw_net {

// +--+--+--+
// / QNAME  /
// +--+--+--+
// | QTYPE  |
// +--+--+--+
// | QCLASS |
// +--+--+--+

struct DnsQuestion {
	std::string name;
	DnsRecordType type = DnsRecordType::Invalid;
	DnsRecordClass cls = DnsRecordClass::Internet;

    const static uint16_t minSize = 6;
};

} // namespace uvw_net
