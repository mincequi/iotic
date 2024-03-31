#include "DnsRecordDataSrv.h"

#include <cstring>
#include <vector>
#include <arpa/inet.h>

#include "DnsParser.h"

namespace uvw_net {

std::optional<DnsRecordDataSrv> DnsRecordDataSrv::fromBuffer(const uint8_t* head,/*const DnsLabels& labels,*/ const uint8_t* data, size_t size) {
    if (size < 8) return {};

    std::vector<uint8_t> vec;
    vec.resize(size);
    std::memcpy(vec.data(), data, size);

    DnsRecordDataSrv ret;
    ret.priority = ntohs(*(uint16_t*)data);
    ret.weight = ntohs(*(uint16_t*)(data+2));
    ret.port = ntohs(*(uint16_t*)(data+4));

    auto [ok, name] = DnsParser::parseName(head, data+6, data+size);
    if (ok) ret.target = name;

    /*
    const auto length = *(data+6);
    ret.target.resize(length);
    std::memcpy(ret.target.data(), (data+7), length);
    */
    return ret;
}

} // namespace uvw_net
