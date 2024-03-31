#pragma once

namespace uvw_net {

enum class DnsOpCode : uint16_t {
};

enum class DnsResponseCode : uint16_t {
};

#pragma pack(1)
struct DnsHeader {
	uint16_t transactionId = 0;

	uint8_t isRecursionDesired : 1;
	uint8_t isTruncated : 1;
	uint8_t isAuthoritative : 1;
	uint8_t opCode : 4;
	uint8_t isResponse : 1;

	uint8_t responseCode : 4;
    uint8_t isCheckingDisabled : 1;
	uint8_t isAuthenticatedData : 1;
	uint8_t reserved : 1;
	uint8_t isRecursionAvailable : 1;
};
#pragma pack()

} // namespace uvw_net
