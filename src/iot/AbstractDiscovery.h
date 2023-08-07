#pragma once

#include <memory>
#include <QObject>

class ThingInfo;
class ThingManager;

class AbstractDiscovery : public QObject {
    Q_OBJECT
public:
    explicit AbstractDiscovery(ThingManager& thingManager, QObject *parent = nullptr);

    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    void onThingDiscovered(const ThingInfo& info);

private:
    ThingManager& _thingManager;
};

using AbstractDiscoveryPtr = std::unique_ptr<AbstractDiscovery>;
