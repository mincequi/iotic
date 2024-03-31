#include "ModbusDiscovery.h"

#include <uvw/util.h>

#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>

using namespace std::placeholders;
using namespace sunspec;

ModbusDiscovery::ModbusDiscovery() {
}

void ModbusDiscovery::start(int msec) {
    for (const auto& c : _candidates) {
        c.second.unsubscribe();
    }
    _candidates.clear();

    std::string subnet;
    const auto interfaces = uvw::utilities::interface_addresses();
    for (const auto& interface : interfaces) {
        if (interface.netmask.ip != "255.255.255.0") continue;

        const auto ip = interface.address.ip;
        subnet = ip.substr(0, ip.find_last_of(".")+1);
        break;
    }

    if (subnet.empty()) {
        LOG_S(WARNING) << "No interface with subnet mask 255.255.255.0 found.";
        return;
    }

    // Scan subnets
    LOG_S(1) << "find things> subnet: " << subnet << "0/24";
    for (uint8_t i = 1; i < 255; ++i) {
        const std::string host = subnet + std::to_string(i);
        if (repo->thingByHost(host)) {
            continue;
        }

        auto candidate = std::make_shared<ModbusThing>(ThingInfo{ThingInfo::SunSpec, host, host});
        auto sub = candidate->stateObservable().subscribe(std::bind(&ModbusDiscovery::onCandidateStateChanged, this, candidate, _1));
        candidate->connect();
        _candidates.push_back({std::move(candidate), sub});
    }
}

void ModbusDiscovery::stop() {
}

void ModbusDiscovery::onCandidateStateChanged(const ModbusThingPtr_asio& candidate_, Thing::State state) {
    assert(!_candidates.empty());
    auto it = std::find_if(_candidates.begin(), _candidates.end(), [&](const auto& c) {
        return c.first == candidate_;
    });
    assert(it != _candidates.end());

    // Steal candidate from container
    auto candidate = std::move(*it);
    _candidates.erase(it);

    // Disconnect signals, since we are handing off this object
    candidate.second.unsubscribe();

    switch (state) {
    case Thing::State::Failed:
        break;
    case Thing::State::Ready: {
        LOG_S(INFO) << "thing found> host: " << candidate_->host();
        // Prepare thing
        auto thing = std::move(candidate.first);
        thingDiscoveredSubscriber().on_next(thing);
        break;
    }
    default:
        break;
    }
}
