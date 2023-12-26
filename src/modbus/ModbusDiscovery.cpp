#include "ModbusDiscovery.h"

#include <QHostAddress>
#include <QNetworkInterface>

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

    // Find network interfaces
    QString subnet;
    const QHostAddress localhost = QHostAddress(QHostAddress::LocalHost);
    foreach (const auto& address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
             subnet = address.toString().left(address.toString().lastIndexOf(".")+1);
        }
    }

    // Scan subnets
    LOG_S(1) << "find things> subnet: " << subnet << "0/24";
    for (uint8_t i = 1; i < 255; ++i) {
        const QString host = subnet + QString::number(i);
        if (repo->thingByHost(host.toStdString())) {
            continue;
        }

        //auto candidate = std::make_shared<ModbusThing>(ThingInfo{ThingInfo::SunSpec, host.toStdString(), host.toStdString()});
        auto candidate = std::make_shared<ModbusThing>(ThingInfo{ThingInfo::SunSpec, host.toStdString(), host.toStdString()});
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
