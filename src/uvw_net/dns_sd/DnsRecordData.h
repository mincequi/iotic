#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

#include <uvw_net/dns_sd/DnsRecordDataSrv.h>
#include <uvw_net/dns_sd/DnsRecordType.h>

namespace uvw_net {

using OctectStreamData = std::vector<std::byte>;

using AData = uint32_t;

using AAAAData = std::array<std::byte, 16>;

using PTRData = std::string;

struct MXData {
	uint16_t preference = 0;
	std::string exchange;
};

struct TXTData {
	uint8_t size = 0;
	std::string txt;
};

struct SOAData {
	std::string primaryServer;
	std::string administrator;
	uint32_t serialNo = 0;
	uint32_t refresh = 0;
	uint32_t retry = 0;
	uint32_t expire = 0;
	uint32_t defaultTtl = 0;
};

using DnsRecordData = std::variant<
		OctectStreamData,
		AData,
		//AAAAData,
		PTRData,
		MXData,
		TXTData,
		SOAData,
        DnsRecordDataSrv>;

} // namespace uvw_net
