#pragma once

#include <QTimer>
#include <things/ThingsDiscovery.h>

class AppDiscover {
public:
    AppDiscover();

private:
    std::list<ThingsDiscoveryPtr> _discoveries;

    QTimer _timer;
};
