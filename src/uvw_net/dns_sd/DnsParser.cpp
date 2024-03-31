#include "DnsParser.h"

#include <cassert>
#include <arpa/inet.h>

namespace uvw_net {

std::tuple<size_t, std::string> DnsParser::parseName(const uint8_t *head, const uint8_t *ptr, const uint8_t *tail) {
    // is ROOT name?
    if (*ptr == 0) {
        return { 1, {} };
    }

    // collect them
    std::vector<const uint8_t*> labels;
    auto dataLen = parseLabels(head, ptr, tail, labels);
    if (!dataLen) {
        return {};
    }

    // concatenate each part
    std::string result;
    for (auto p : labels) {
        result += std::string((char *)p + 1, *p);
        result += ".";
    }

    // done
    return { dataLen, std::move(result) };
}

int DnsParser::parseLabels(const uint8_t *head, const uint8_t *ptr, const uint8_t *tail, std::vector<const uint8_t *> &parts, bool recursively) {
    assert(head != nullptr && ptr != nullptr && tail != nullptr);
    assert(head < tail);
    if (ptr < head) {
        return 0;
    }
    if (ptr >= tail) {
        return 0;
    }

    bool countTerminal = true;
    int consumed = 0;
    while (*ptr && ptr < tail) {
        if (((uint8_t)*ptr & 0xc0) != 0) {
            // compress pointer
            uint16_t offset = htons(*(uint16_t*)ptr) & (uint16_t)(~0xc000);
            if (head + offset >= tail) {
                // illegal
                return 0;
            }
            if (recursively) {
                // recursive collect
                if (!parseLabels(head, head + offset, tail, parts)) {
                    // illegal
                    return 0;
                }
            }

            // consumed += sizeof(uint16_t)
            consumed += 2;
            countTerminal = false;
            ptr += 2;

            // the compressed part should be the last part of a host name
            break;
        } else if (*ptr >= 64 || ptr + *ptr > tail) {
            // illegal
            return 0;
        } else {
            // plain text
            parts.emplace_back(ptr);

            // consumed += [len] [text]
            consumed += *ptr + 1;

            // move ptr
            ptr += (*ptr) + 1;
        }
    }
    if (countTerminal) {
        ++consumed;
    }

    // we should count the last null terminator in
    return consumed;
}

} // namespace uvw_net
