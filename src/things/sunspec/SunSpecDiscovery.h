#include <modbus/ModbusDiscovery.h>
#include <things/sunspec/SunSpecThing.h>

class SunSpecDiscovery : public ThingsDiscovery {
public:
    SunSpecDiscovery(ModbusDiscoveryPtr modbusDiscovery);

private:
    void onCandidateStateChanged(const SunSpecThingPtr& thing, Thing::State state);

    ModbusDiscoveryPtr _modbusDiscovery;
    std::list<std::pair<SunSpecThingPtr, rpp::composite_subscription>> _candidates;
};
