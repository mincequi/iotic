#include "CoapMessage.h"

#include <algorithm>

#include "CoreLinkDocument.h"

std::optional<CoapMessage> CoapMessage::decode(std::string_view data, const std::string& senderAddress, uint16_t senderPort) {
    CoapMessage msg;

    // Need at least 4 bytes for header
    if (data.size() < 4) return std::nullopt;

    const uint8_t *p = reinterpret_cast<const uint8_t*>(data.data());
    size_t remaining = data.size();

    // Header
    uint8_t hdr     = p[0];
    msg.version     = (hdr >> 6) & 0x03;
    msg.type        = static_cast<Type>((hdr >> 4) & 0x03);
    const uint8_t tokenLength = hdr & 0x0F;
    msg.code        = static_cast<Code>(p[1]);
    msg.messageId   = static_cast<uint16_t>(p[2] << 8 | p[3]);

    p += 4;
    remaining -= 4;

    // max TKL is actually 8, but extended Coap allows up to 12
    if (tokenLength > 12) return std::nullopt; // TKL is ac
    if (remaining < tokenLength) return std::nullopt;

    // Token
    msg.token.assign(p, p + tokenLength);
    p += tokenLength;
    remaining -= tokenLength;

    // Options
    uint16_t currentOptionNumber = 0;

    while (remaining > 0) {
        if (*p == 0xFF) {
            // Payload marker
            p++;
            remaining--;
            break;
        }

        if (remaining < 1) return std::nullopt;

        uint8_t optHdr = *p++;
        remaining--;

        const uint8_t deltaNibble = (optHdr >> 4) & 0x0F;
        const uint8_t lengthNibble = optHdr & 0x0F;

        // Decode extended delta
        uint16_t delta = 0;
        if (deltaNibble < 13) {
            delta = deltaNibble;
        } else if (deltaNibble == 13) {
            if (remaining < 1) return std::nullopt;
            delta = 13 + *p;
            p++; remaining--;
        } else if (deltaNibble == 14) {
            if (remaining < 2) return std::nullopt;
            delta = 269 + (p[0] << 8 | p[1]);
            p += 2; remaining -= 2;
        } else {
            return std::nullopt; // reserved
        }

        // Decode extended length
        uint16_t length = 0;
        if (lengthNibble < 13) {
            length = lengthNibble;
        } else if (lengthNibble == 13) {
            if (remaining < 1) return std::nullopt;
            length = 13 + *p;
            p++; remaining--;
        } else if (lengthNibble == 14) {
            if (remaining < 2) return std::nullopt;
            length = 269 + (p[0] << 8 | p[1]);
            p += 2; remaining -= 2;
        } else {
            return std::nullopt; // reserved
        }

        currentOptionNumber += delta;

        if (remaining < length) return std::nullopt;

        //CoapMessage::Option opt;
        //opt.key = static_cast<OptionKey>(currentOptionNumber);
        //opt.value.assign(p, p + length);
        //msg.options.push_back(std::move(opt));

        msg.optionsMap.emplace(static_cast<OptionKey>(currentOptionNumber), std::vector<uint8_t>(p, p + length));

        p += length;
        remaining -= length;
    }

    // Payload (if any)
    msg.payload.assign(p, p + remaining);
    msg.senderAddress = senderAddress;
    msg.senderPort = senderPort;

    return msg;
}

std::vector<uint8_t> CoapMessage::encode() const {
    std::vector<uint8_t> out;
    out.reserve(64);

    // Mandatory Header
    const uint8_t token_len = token.size();
    if (token_len > 12) {
        return {};
    }

    const uint8_t vtt =
        (version << 6) |
        (static_cast<uint8_t>(type) << 4) |
        (token_len & 0x0F);

    // Fixed 4-byte header
    out.push_back(vtt);
    out.push_back(static_cast<uint8_t>(code));
    out.push_back(messageId >> 8);
    out.push_back(messageId & 0xFF);

    // Token
    out.insert(out.end(), token.begin(), token.begin() + token_len);

    // Options (must be delta encoded and sorted)
    std::vector<Option> sorted = options;
    std::sort(sorted.begin(), sorted.end(), [](auto &a, auto &b){ return (uint16_t)a.key < (uint16_t)b.key; });

    uint16_t optionNumber = 0;
    for (auto& opt : options) {
        uint16_t number = (uint16_t)opt.key;
        uint16_t delta = number - optionNumber;
        optionNumber = number;

        uint16_t length = opt.value.size();
        auto encode_extended = [&](uint16_t x, uint8_t &base, std::vector<uint8_t> &extra){
            if (x < 13) base = x;
            else if (x < 269) { base = 13; extra.push_back(x - 13); }
            else {
                base = 14;
                extra.push_back((x - 269) >> 8);
                extra.push_back((x - 269) & 0xFF);
            }
        };

        uint8_t base_delta = 0, base_len = 0;
        std::vector<uint8_t> ext;

        encode_extended(delta, base_delta, ext);
        encode_extended(length, base_len, ext);

        out.push_back((base_delta << 4) | base_len);

        out.insert(out.end(), ext.begin(), ext.end());
        out.insert(out.end(), opt.value.begin(), opt.value.end());
    }

    // Payload
    if (!payload.empty()) {
        out.push_back(0xFF); // payload marker
        out.insert(out.end(), payload.begin(), payload.end());
    }

    return out;
}

CoapMessage CoapMessage::buildDiscoveryRequest() {
    CoapMessage msg;
    msg.code = Code::Get;
    // Add Uri-Path: /.well-known/core
    msg.options.push_back({CoapMessage::OptionKey::UriPath, {'.', 'w', 'e', 'l', 'l', '-', 'k', 'n', 'o', 'w', 'n'}});
    msg.options.push_back({CoapMessage::OptionKey::UriPath, {'c', 'o', 'r', 'e'}});

    return msg;
}

bool CoapMessage::isDiscoveryResponse() const {
    // Check for Content code
    if (code != Code::Content) return false;

    // Check if ContentFormat is application/link-format (40)
    auto it = optionsMap.find(OptionKey::ContentFormat);
    if (it == optionsMap.end()) return false;
    if (it->second.size() != 1) return false;
    if (it->second[0] != static_cast<uint8_t>(ContentFormat::ApplicationLinkFormat)) return false;

    coreLinkDocument = CoreLinkDocument::decode(std::string_view((char*)payload.data(), payload.size()));
    if (!coreLinkDocument) return false;

    return true;
}
