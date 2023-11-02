#pragma once

#include <memory>

#include <things/Thing.h>

class Strategy;

class StrategyFactory {
public:
    static std::unique_ptr<Strategy> from(const ThingPtr& thing);
};
