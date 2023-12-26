#include "SunSpecDiscovery.h"

#include <assert.h>

#include <common/Logger.h>
#include <config/Config.h>
#include <modbus/ModbusThing.h>
#include <rpp/operators/filter.hpp>

using namespace std::placeholders;
using namespace sunspec;

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
    assert(!_candidates.empty());
    auto it = std::find_if(_candidates.begin(), _candidates.end(), [&](const auto& c) {
        return c.first == candidate_;
    });
    assert(it != _candidates.end());

    // Steal candidate from container
    auto candidate = *it;
    _candidates.erase(it);

    // Disconnect signals, since we are handing off this object
    candidate.second.unsubscribe();

    switch (state) {
    case Thing::State::Failed:
        break;
    case Thing::State::Ready: {
        // Prepare thing
        auto thing = std::move(candidate.first);
        std::stringstream supportedModels;
        std::stringstream unsupportedModels;
        for (const auto& kv : candidate_->models()) {
            auto m = magic_enum::enum_cast<Model::Id>(kv.first);
            if (m.has_value())
                supportedModels << magic_enum::enum_name(m.value())  << " (" << kv.first << "), ";
            else
                unsupportedModels << kv.first << ", ";
        }
        LOG_S(INFO) << "thing found> id: " << thing->sunSpecId()
                    << ", host: " << candidate_->host()
                    << ", unitId: " << (uint32_t)thing->unitId()
                    << ", supportedModels: " << supportedModels.str()
                    << "unsupportedModels: " << unsupportedModels.str();
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
