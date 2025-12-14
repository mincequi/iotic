#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct CoapMessage {
    // Types
    enum class Type : uint8_t {
        Confirmable     = 0,
        NonConfirmable  = 1,
        Acknowledgement = 2,
        Reset           = 3
    };

    enum class Code : uint8_t {
        Empty  = 0x00,
        // Methods
        Get    = 0x01,
        Post   = 0x02,
        Put    = 0x03,
        Delete = 0x04,
        // Response codes (examples)
        Content            = 0x45,  // 2.05
        BadRequest         = 0x80,  // 4.00
        NotFound           = 0x84,  // 4.04
        InternalServerError = 0xA0  // 5.00
    };

    enum class OptionKey : uint16_t {
        IfMatch       = 1,
        UriHost       = 3,
        ETag          = 4,
        IfNoneMatch   = 5,
        Observe       = 6,
        UriPort       = 7,
        LocationPath  = 8,
        UriPath       = 11,
        ContentFormat = 12,
        MaxAge        = 14,
        UriQuery      = 15,
        Accept        = 17,
        LocationQuery = 20,
        ProxyUri      = 35,
        ProxyScheme   = 39,
        Size1         = 60
    };

    enum class ContentFormat : uint16_t {
        TextPlain       = 0,
        ApplicationLinkFormat = 40,
        ApplicationJson = 50,
        ApplicationCbor = 60
    };

    // Mandatory fields in order of appearance in the message
    uint8_t version = 1;
    Type type = Type::NonConfirmable;
    Code code = Code::Empty;
    uint16_t messageId = 0;

    // Optional fields in order of appearance in the message
    std::vector<uint8_t> token;
    struct Option {
        OptionKey key;
        std::vector<uint8_t> value;
    };
    std::vector<Option> options;
    std::vector<uint8_t> payload;

    static std::optional<CoapMessage> decode(std::string_view data);
    std::vector<uint8_t> encode() const;
};
