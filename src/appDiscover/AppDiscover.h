#pragma once

#include <things/sunspec/SunSpecManager.h>

using namespace sunspec;

class AppDiscover {
public:
    AppDiscover();

private:
    std::list<ThingsDiscoveryPtr> _discoveries;
    //sunspec::SunSpecManager _sunSpecManager;

    QTimer _timer;
};
