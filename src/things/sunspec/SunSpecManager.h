#pragma once

#include <QTimer>

#include <modbus/ModbusDiscovery.h>
#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/SunSpecThing.h>

namespace sunspec {

class SunSpecManager : public QObject {
    Q_OBJECT
public:
    explicit SunSpecManager(ThingsRepository& repository,
                            QObject *parent = nullptr);

    void startDiscovery(uint16_t seconds = 60);

    bool contains(const QString& host) const;

signals:
    void thingDiscovered(const sunspec::SunSpecThing&);
    void thingDisappeared(const sunspec::SunSpecThing&);

    void modelRead(const SunSpecThing& thing, const sunspec::Model& model, uint32_t timestamp);

    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    void endOfDayReached();

private:
    void removeThing(SunSpecThing*);
    void onTimer();
    //void onThingStateChanged(SunSpecThing::State state);

    modbus::ModbusDiscovery _discovery;

    QMap<std::string, sunspec::SunSpecThing*> _things;

    QTimer _timer;
    uint64_t _currentTimestamp = 0;
};

} // namespace sunspec
