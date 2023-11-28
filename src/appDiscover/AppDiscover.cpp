#include "AppDiscover.h"

#include <common/Util.h>
#include <modbus/ModbusDiscovery.h>
#include <things/http/HttpDiscovery.h>
#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModel.h>

#include <QCoreApplication>
#include <QThread>

using namespace sunspec;
using namespace std::chrono_literals;

AppDiscover::AppDiscover()
    //: _sunSpecManager(*_thingsRepository) {
    {

    _discoveries.push_back(std::make_unique<HttpDiscovery>());
    _discoveries.push_back(std::make_unique<modbus::ModbusDiscovery>());

    for (const auto& d : _discoveries) {
        d->thingDiscovered().subscribe([](const ThingInfo&) {
        });
    }

    for (const auto& d : _discoveries) {
        d->start(70000);
    }

    // Start discovery
    //_sunSpecManager.startDiscovery(60 * 1000);

    QObject::connect(&_timer, &QTimer::timeout, []() {
        qApp->exit();
    });
    _timer.start(60s);
}

