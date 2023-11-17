#include "ModbusDiscovery.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>

#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingsRepository.h>
#include <things/sunspec/SunSpecManager.h>
#include <things/sunspec/SunSpecThing.h>

namespace modbus {

using namespace std::placeholders;
using namespace sunspec;

ModbusDiscovery::ModbusDiscovery(//SunSpecManager& manager,
                                 ThingsRepository& repository) :
    //_manager(manager),
    _thingsRepository(repository) {
    _discoveryTimer.callOnTimeout(this, &ModbusDiscovery::onStartDiscovering);
}

void ModbusDiscovery::start(int msec) {
    onStartDiscovering();
    _discoveryTimer.start(msec);
}

void ModbusDiscovery::stop() {
    _discoveryTimer.stop();
}

void ModbusDiscovery::onStartDiscovering() {
    foreach (auto thing, _candidates) {
        thing.second.unsubscribe();
        delete thing.first;
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
    QTcpSocket socket;
    LOG_S(INFO) << "discovering things in subnet: " << subnet << "0/24";
    for (uint8_t i = 1; i < 255; ++i) {
        const QString host = subnet + QString::number(i);
        if (_thingsRepository.thingByHost(host.toStdString())) {
            continue;
        }
        auto* candidate = new SunSpecThing({ ThingInfo::SunSpec, host.toStdString(), host.toStdString() });
        //candidate->connect(candidate, &SunSpecThing::stateChanged, this, &ModbusDiscovery::onCandidateStateChanged);
        auto sub = candidate->state().subscribe(std::bind(&ModbusDiscovery::onCandidateStateChangedRpp, this, candidate, _1));
        _candidates.append({candidate, sub});
        candidate->connectDevice();
    }
}

void ModbusDiscovery::onCandidateStateChangedRpp(sunspec::SunSpecThing* candidate, sunspec::SunSpecThing::State state) {
    switch (state) {
    case SunSpecThing::State::Failed:
        _candidates.removeIf([&](const auto& c) { return c.first == candidate; } );
        delete candidate;
        break;
    case SunSpecThing::State::Connected:
        // Disconnect signals, since we are handing off this object
        //candidate->disconnect();
        //_candidates.removeAll(candidate);
        _candidates.removeIf([&](const auto& c) { if (c.first == candidate) {
                c.second.unsubscribe();
                return true;
            } return false; } );
        //_manager.addThing(candidate);
        std::stringstream ss;
        for (const auto& kv : candidate->models()) {
            ss << kv.first << kv.second.second << ", ";
        }
        LOG_S(INFO) << "thing discovered> id: " << candidate->sunSpecId()
                    << ", host: " << candidate->host()
                    << ", modbusUnitId: " << (uint32_t)candidate->modbusUnitId()
                    << ", models: " << ss.str();
        candidate->_id = candidate->sunSpecId();
        candidate->setProperty(MutableProperty::pinned, cfg->valueOr(candidate->sunSpecId(), Config::Key::pinned, false));
        candidate->setProperty(MutableProperty::name, cfg->valueOr(candidate->sunSpecId(), Config::Key::name, candidate->sunSpecId()));
        _thingsRepository.addThing(ThingPtr(candidate));
        break;
    }
}

} // namespace modbus
