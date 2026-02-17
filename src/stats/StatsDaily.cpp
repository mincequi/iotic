#include "StatsDaily.h"

#include <uvw_iot/ThingRepository.h>

using namespace uvw_iot;

StatsDaily::StatsDaily(const ThingRepository& thingRepository) {

    thingRepository.thingAdded().subscribe([this](uvw_iot::ThingPtr thing) {
        if (thing->type() != ThingType::SolarInverter) return;

        thing->propertiesObservable().subscribe([this, thing](const auto& props) {
            props.template on<ThingPropertyKey::dcPower>([this, thing](const std::vector<int>& value) {
                const auto now = std::chrono::system_clock::now();
                const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
                const int day = seconds / (24 * 3600);
                ThingStats& stats = _stats[thing->id()];
                if (day != stats.currentDay) {
                    stats.currentDay = day;
                    stats.energyDelivered = std::vector<int>(value.size(), 0);
                    stats.timestamp = seconds;
                }

                for (size_t i = 0; i < value.size(); ++i) {
                    stats.energyDelivered[i] += value[i] * (seconds - stats.timestamp);
                }
                stats.timestamp = seconds;

                // Convert Ws into kWm
                std::vector<int> energyDelivered(value.size());
                for (size_t i = 0; i < value.size(); ++i) {
                    energyDelivered[i] = stats.energyDelivered[i] / 60000;
                }
                ThingPropertyMap map;
                map.set<ThingPropertyKey::dcEnergy>(energyDelivered);
                thing->setProperties(map);
            });
        });
    });
}
