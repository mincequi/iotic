#include "EvseStrategy.h"

#include <numeric>

#include <magic_enum.hpp>

#include <rpp/operators/debounce.hpp>
#include <rpp/operators/distinct_until_changed.hpp>
#include <rpp/operators/observe_on.hpp>
#include <rpp/schedulers/new_thread_scheduler.hpp>
#include <rppqt/schedulers/main_thread_scheduler.hpp>

#include <common/Logger.h>
#include <config/Config.h>

EvseStrategy::EvseStrategy(const std::string& thingId) :
    Strategy(thingId) {
    _phasesSubject.get_observable()
            .distinct_until_changed()
            .debounce(std::chrono::milliseconds(cfg->valueOr<int>(thingId, Config::Key::debounce, 300000)), rpp::schedulers::new_thread())
            .observe_on(rppqt::schedulers::main_thread_scheduler{})
            .subscribe([this](const Phases& phases) {
        LOG_S(INFO) << this->thingId() << "> " << phases;
        _phasesState = phases;
    });
}

EvseStrategy::~EvseStrategy() {
}

void EvseStrategy::setChargingData(const EvseData& data) {
    _chargingData = data;
}

void EvseStrategy::setGridPower(int watts) {
    _gridPower = watts;
}

void EvseStrategy::setOffsetPower(int watts) {
    _offsetPower = watts;
}

void EvseStrategy::evaluate() {
    int availablePower = _chargingData.power - _gridPower + _offsetPower;

    const double averageVoltage = std::reduce(_chargingData.voltages.begin(), _chargingData.voltages.end()) / _chargingData.voltages.size();

    Phases phases = Phases::Off;
    if (availablePower > (3*6*averageVoltage)) {
        phases = Phases::ThreePhase;
    } else if (availablePower > (6*_chargingData.voltages.front())) {
        phases = Phases::SinglePhase;
    }

    // Set initial value
    if (!_phasesState.has_value()) _phasesState = phases;

    // Trigger subject
    _phasesSubject.get_subscriber().on_next(phases);

    //
}
