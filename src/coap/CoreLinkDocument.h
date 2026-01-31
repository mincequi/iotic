#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>

enum class CoreLinkResourceType {
    temperature,
    humidity,
    light,
    switch_,
    percentage,
    voltage,
    current,
    power,
};

struct CoreLinkDocument {
    std::multimap<CoreLinkResourceType, std::string> sensors;
    std::multimap<CoreLinkResourceType, std::string> actuators;

    static std::optional<CoreLinkDocument> decode(std::string_view data);
    static void forEach(std::string_view sv, char c, const std::function<void(std::size_t index, std::string_view)>& fn);
};
