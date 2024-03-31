#pragma once

#include <cstdint>

namespace uvw_net {

enum DnsRecordClass : uint16_t {
    Invalid = 0,

	Internet = 1,
	CHAOS = 3,
	Hesiod = 4,
};

} // namespace uvw_net
