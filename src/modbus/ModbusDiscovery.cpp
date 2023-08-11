#include "ModbusDiscovery.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>

#include <common/Logger.h>
#include <things/sunspec/SunSpecManager.h>
#include <things/sunspec/SunSpecThing.h>

namespace modbus {

using namespace sunspec;

ModbusDiscovery::ModbusDiscovery(SunSpecManager& manager) :
    _manager(manager) {
    _discoveryTimer.callOnTimeout(this, &ModbusDiscovery::onStartDiscovering);
}

void ModbusDiscovery::start(uint16_t seconds) {
    onStartDiscovering();
    _discoveryTimer.start(seconds * 1000);
}

void ModbusDiscovery::stop() {
    _discoveryTimer.stop();
}

void ModbusDiscovery::onStartDiscovering() {
    foreach (auto thing, _candidates) {
        thing->deleteLater();
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
        if (_manager.contains(host)) {
            continue;
        }
        auto* candidate = new SunSpecThing(host);
        _candidates.append(candidate);
        candidate->connect(candidate, &SunSpecThing::stateChanged, this, &ModbusDiscovery::onCandidateStateChanged);
        candidate->connectDevice();
    }
}

void ModbusDiscovery::onCandidateStateChanged(SunSpecThing::State state) {
    auto candidate = qobject_cast<SunSpecThing*>(sender());
    if (!candidate) {
        qFatal("IMPOSSIBLE");
    }
    switch (state) {
    case SunSpecThing::State::Failed:
        // If connection failed, delete candidate
        _candidates.removeOne(candidate);
        candidate->deleteLater();
        break;
    case SunSpecThing::State::Connected:
        // Disconnect signals, since we are handing off this object
        candidate->disconnect();
        _candidates.removeAll(candidate);
        _manager.addThing(candidate);
        break;
    }
}

} // namespace modbus
