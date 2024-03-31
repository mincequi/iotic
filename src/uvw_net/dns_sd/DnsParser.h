#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace uvw_net {

class DnsParser {
public:
    static std::tuple<size_t, std::string> parseName(const uint8_t *head,
                                                     const uint8_t *ptr,
                                                     const uint8_t *tail);
private:
    static int parseLabels(const uint8_t *head,
                           const uint8_t *ptr,
                           const uint8_t *tail,
                           std::vector<const uint8_t *> &parts,
                           bool recursively = true);
};

} // namespace uvw_net
