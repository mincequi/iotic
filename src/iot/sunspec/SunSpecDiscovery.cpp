#include "SunSpecDiscovery.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>

#include <common/Logger.h>
#include <sunspec/SunSpecManager.h>
#include <sunspec/SunSpecThing.h>

namespace sunspec {

SunSpecDiscovery::SunSpecDiscovery(SunSpecManager& manager) :
    _manager(manager) {
    _discoveryTimer.callOnTimeout(this, &SunSpecDiscovery::onStartDiscovering);
}

void SunSpecDiscovery::start(uint16_t seconds) {
    onStartDiscovering();
    _discoveryTimer.start(seconds * 1000);
}

void SunSpecDiscovery::stop() {
    _discoveryTimer.stop();
}

void SunSpecDiscovery::removeThing(SunSpecThing *thing, bool all) {
    // TODO: no idea, why i did this
    if (all)
        _discoveringThings.removeAll(thing);
    else
        _discoveringThings.removeOne(thing);
}

void SunSpecDiscovery::onStartDiscovering() {
    foreach (auto thing, _discoveringThings) {
        thing->deleteLater();
    }
    _discoveringThings.clear();

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
        auto* thing = new sunspec::SunSpecThing(host);
        _discoveringThings.append(thing);
        thing->connect(thing, &sunspec::SunSpecThing::stateChanged, this, &SunSpecDiscovery::onThingStateChanged);
        thing->connectDevice();
    }
}

void SunSpecDiscovery::onThingStateChanged(SunSpecThing::State state) {
    auto thing = qobject_cast<SunSpecThing*>(sender());
    if (!thing) {
        qFatal("IMPOSSIBLE");
    }
    switch (state) {
    case SunSpecThing::State::Failed:
        removeThing(thing);
        break;
    case SunSpecThing::State::Connected:
        removeThing(thing, true);
        _manager.addThing(thing);
        break;
    }
}

} // namespace sunspec
