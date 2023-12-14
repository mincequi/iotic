#include "ModbusDiscovery.h"

#include <QHostAddress>
#include <QNetworkInterface>

#include <common/Logger.h>
#include <config/Config.h>
#include <modbus/ModbusThing.h>
#include <things/ThingsRepository.h>

namespace modbus {

using namespace std::placeholders;
using namespace sunspec;

ModbusDiscovery::ModbusDiscovery() {
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
    // TODO: do we actually need to unsubscribe?
    //for (const auto& c : _candidates) {
    //    c.second.unsubscribe();
    //}
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
    LOG_S(INFO) << "discovering things in subnet: " << subnet << "0/24";
    for (uint8_t i = 1; i < 255; ++i) {
        const QString host = subnet + QString::number(i);
        if (repo->thingByHost(host.toStdString())) {
            continue;
        }

        auto candidate = std::make_unique<SunSpecThing>(ThingInfo{ThingInfo::SunSpec, host.toStdString(), host.toStdString()});
        //auto candidate = std::make_shared<ModbusThing>(ThingInfo{ThingInfo::SunSpec, host.toStdString(), host.toStdString()});
        auto sub = candidate->stateObservable().subscribe(std::bind(&ModbusDiscovery::onSunspecCandidateStateChanged, this, candidate.get(), _1));
        candidate->connect();
        _candidates.push_back({std::move(candidate), sub});
    }
}

void ModbusDiscovery::onCandidateStateChanged(const ModbusThingPtr& candidate_, Thing::State state) {
    /*
    switch (state) {
    case Thing::State::Failed:
        // TODO: do we actually need to unsubscribe?
        _candidates.remove_if([&](const auto& c) {
            return c.first == candidate_;
        });
        break;
    case Thing::State::Ready: {
        // Steal candidate from container
        auto it = std::find_if(_candidates.begin(), _candidates.end(), [&](const auto& c) {
            return c.first == candidate_;
        });

        auto candidate = std::move(*it);
        _candidates.erase(it);

        // Disconnect signals, since we are handing off this object
        candidate.second.unsubscribe();

        // Prepare thing
        auto thing = std::move(candidate.first);
        std::stringstream ss;

        for (const auto& kv : thing->models()) {
            ss << kv.first << ", ";
        }
        LOG_S(INFO) << "thing discovered> id: " << thing->sunSpecId()
                    << ", host: " << thing->host()
                    << ", modbusUnitId: " << (uint32_t)thing->modbusUnitId()
                    << ", models: " << ss.str();
        thing->_id = thing->sunSpecId();

        // TODO: Remove dependency to thingsRepository
        thing->setProperty(MutableProperty::pinned, cfg->valueOr(thing->sunSpecId(), Config::Key::pinned, false));
        thing->setProperty(MutableProperty::name, cfg->valueOr(thing->sunSpecId(), Config::Key::name, thing->sunSpecId()));
        //repo->addThing(std::move(thing));
        thingDiscoveredSubscriber().on_next(thing);

        break;
    }
    default:
        break;
    }
    */
}

void ModbusDiscovery::onSunspecCandidateStateChanged(const SunSpecThing* candidate_, Thing::State state) {
    switch (state) {
    case Thing::State::Failed:
        // TODO: do we actually need to unsubscribe?
        _candidates.remove_if([&](const auto& c) {
            return c.first.get() == candidate_;
        });
        break;
    case Thing::State::Ready: {
        // Steal candidate from container
        auto it = std::find_if(_candidates.begin(), _candidates.end(), [&](const auto& c) {
            return c.first.get() == candidate_;
        });

        auto candidate = std::move(*it);
        _candidates.erase(it);

        // Disconnect signals, since we are handing off this object
        candidate.second.unsubscribe();

        // Prepare thing
        auto thing = std::move(candidate.first);
        std::stringstream ss;
        for (const auto& kv : thing->models()) {
            ss << kv.first << ", ";
        }
        LOG_S(INFO) << "thing discovered> id: " << thing->sunSpecId()
                    << ", host: " << thing->host()
                    << ", modbusUnitId: " << (uint32_t)thing->modbusUnitId()
                    << ", models: " << ss.str();
        thing->_id = thing->sunSpecId();

        // TODO: Remove dependency to thingsRepository
        thing->setProperty(MutableProperty::pinned, cfg->valueOr(thing->sunSpecId(), Config::Key::pinned, false));
        thing->setProperty(MutableProperty::name, cfg->valueOr(thing->sunSpecId(), Config::Key::name, thing->sunSpecId()));
        //repo->addThing(std::move(thing));
        thingDiscoveredSubscriber().on_next(thing);
        break;
    }
    default:
        break;
    }
}

} // namespace modbus
