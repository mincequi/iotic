#pragma once

#include <QList>
#include <QTimer>
#include <things/ThingsDiscovery.h>
#include "SunSpecThing.h"

namespace sunspec {

class SunSpecThing;
class SunSpecManager;

class SunSpecDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    SunSpecDiscovery(SunSpecManager& manager);

    void start(uint16_t seconds = 60) override;
    void stop() override;

    void removeThing(SunSpecThing* thing, bool all = false);

private:
    void onStartDiscovering();
    void onThingStateChanged(sunspec::SunSpecThing::State state);

    SunSpecManager& _manager;
    QTimer _discoveryTimer;
    QList<sunspec::SunSpecThing*> _discoveringThings;
};

} // namespace sunspec
