#include "SunSpecDiscovery.h"

#include <common/Logger.h>
#include <config/Config.h>

using namespace sunspec;
using namespace std::placeholders;

SunSpecDiscovery::SunSpecDiscovery(ModbusDiscoveryPtr modbusDiscovery) :
    _modbusDiscovery(modbusDiscovery) {
    // ModbusDiscovery spawns ready/connected ModbusThings
    _modbusDiscovery->thingDiscovered().subscribe([this](ThingPtr thing) {
        auto candidate = std::make_shared<SunSpecThing>(std::static_pointer_cast<ModbusThing>(thing));
        auto sub = candidate->stateObservable().subscribe(std::bind(&SunSpecDiscovery::onCandidateStateChanged, this, candidate, _1));
        _candidates.push_back({std::move(candidate), sub});
    });
}

void SunSpecDiscovery::onCandidateStateChanged(const SunSpecThingPtr& candidate_, Thing::State state) {
    auto it = std::find_if(_candidates.begin(), _candidates.end(), [&](const auto& c) {
        return c.first == candidate_;
    });
    if (it == _candidates.end()) return;

    // Steal candidate from container
    auto candidate = std::move(*it);
    _candidates.erase(it);

    // Disconnect signals, since we are handing off this object
    candidate.second.unsubscribe();

    switch (state) {
    case Thing::State::Failed:
        break;
    case Thing::State::Ready: {
        // Prepare thing
        auto thing = std::move(candidate.first);
        std::stringstream ss;
        for (const auto& kv : thing->models()) {
            ss << kv.first << ", ";
        }
        LOG_S(INFO) << "thing found> id: " << thing->sunSpecId()
                    << ", host: " << thing->host()
                    << ", modbusUnitId: " << (uint32_t)thing->modbusUnitId()
                    << ", models: " << ss.str();
        thing->_id = thing->sunSpecId();

        thing->setProperty(MutableProperty::pinned, cfg->valueOr(thing->sunSpecId(), Config::Key::pinned, false));
        thing->setProperty(MutableProperty::name, cfg->valueOr(thing->sunSpecId(), Config::Key::name, thing->sunSpecId()));
        thingDiscoveredSubscriber().on_next(thing);
        break;
    }
    default:
        break;
    }
}
