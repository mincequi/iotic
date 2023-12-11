#include "AppDiscover.h"

#include <modbus/ModbusDiscovery.h>
#include <things/http/HttpDiscovery.h>

#include <QCoreApplication>

using namespace std::chrono_literals;

AppDiscover::AppDiscover() {
    _discoveries.push_back(std::make_unique<HttpDiscovery>());
    _discoveries.push_back(std::make_unique<modbus::ModbusDiscovery>());

    for (const auto& d : _discoveries) {
        d->start(70000);
    }

    QObject::connect(&_timer, &QTimer::timeout, []() {
        qApp->exit();
    });
    _timer.start(60s);
}

