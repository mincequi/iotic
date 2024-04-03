#include "AppDiscover.h"

#include <modbus/ModbusDiscovery.h>
#include <things/http/HttpDiscovery.h>

#include <uvw/loop.h>
#include <uvw/timer.h>

using namespace std::chrono_literals;

AppDiscover::AppDiscover() {
    _discoveries.push_back(std::make_unique<HttpDiscovery>());
    _discoveries.push_back(std::make_unique<ModbusDiscovery>());

    for (const auto& d : _discoveries) {
        d->start(70000);
    }

    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([this](const auto&, auto&) {
        uvw::loop::get_default()->stop();
    });
    timer->start(uvw::timer_handle::time{60000}, uvw::timer_handle::time{0});
}

