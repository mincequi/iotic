#pragma once

#include <list>

#include <things/ThingsDiscovery.h>

class AppDiscover {
public:
    AppDiscover();

private:
    std::list<ThingsDiscoveryPtr> _discoveries;
};
