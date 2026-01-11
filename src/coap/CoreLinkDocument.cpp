#include "CoreLinkDocument.h"

#include <magic_enum.hpp>

std::optional<CoreLinkDocument> CoreLinkDocument::decode(std::string_view data) {
    // Placeholder implementation
    if (data.empty()) {
        return std::nullopt;
    }

    CoreLinkDocument doc;
    forEach(data, ',', [&](std::size_t index, std::string_view element) {
        std::string_view ref;
        std::map<std::string_view, std::string_view> attributes;

        forEach(element, ';', [&](std::size_t subIndex, std::string_view subElement) {
            if (subIndex == 0) {
                subElement.remove_prefix(1);
                subElement.remove_suffix(1); // Remove '<' and '>'
                ref = subElement;
            } else if (!ref.empty()) {
                size_t eqPos = subElement.find('=');
                if (eqPos != std::string_view::npos) {
                    std::string_view key = subElement.substr(0, eqPos);
                    std::string_view value = subElement.substr(eqPos + 1);
                    value.remove_prefix(1);
                    value.remove_suffix(1); // Remove quotes
                    attributes[key] = value;
                }
            }
        });

        if (ref.empty() ||
            !attributes.contains("if") ||
            !attributes.contains("rt")) {
            return;
        }

        std::string_view iface = attributes["if"];
        std::string_view rtype = attributes["rt"];
        if (iface != "sensor" && iface != "actuator") {
            return; // Unknown interface
        }

        // Use enum reflection to map resource type strings to enum values
        auto resourceType = magic_enum::enum_cast<CoreLinkResourceType>(rtype);
        if (!resourceType.has_value()) {
            return; // Unknown resource type
        }

        if (iface == "sensor") {
            doc.sensors.emplace(*resourceType, std::string(ref));
        } else if (iface == "actuator") {
            doc.actuators.emplace(*resourceType, std::string(ref));
        }
    });

    if (doc.sensors.empty() && doc.actuators.empty()) {
        return std::nullopt;
    }

    return doc;
}

void CoreLinkDocument::forEach(std::string_view sv, char c, const std::function<void(std::size_t index, std::string_view)>& fn) {
    size_t start = 0;
    std::size_t index = 0;

    while (true) {
        size_t end = sv.find(c, start);
        fn(index, sv.substr(start, end - start));
        if (end == std::string_view::npos)
            break;
        start = end + 1;
        ++index;
    }
}
