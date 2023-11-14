#pragma once

#include <functional>
#include <map>
#include <things/ThingProperty.h>
#include <things/ThingValue.h>

class Router {
public:
    Router();

    bool route(const std::string& thing, MutableProperty property, const Value& value);

private:
    std::map<std::pair<std::string, MutableProperty>, std::function<void(const Value&)>> _routes;
};
