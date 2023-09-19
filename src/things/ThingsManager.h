#pragma once

#include <map>
#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <QObject>
#include <QTimer>
#include <things/Thing.h>
#include <things/ThingsDiscovery.h>

class ThingsRepository;

class ThingsManager : public QObject {
    Q_OBJECT
public:
    struct Task {
        std::string thingId;
        std::chrono::milliseconds intervalMs = std::chrono::milliseconds(0);

        bool operator==(const Task& other);
    };

    explicit ThingsManager(ThingsRepository& thingsRepository, QObject *parent = nullptr);

    void startDiscovery(uint seconds);
    void stopDiscovery();

    void addTask(const Task& task);

signals:
    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    void endOfDayReached();

private:
    void onTimer();
    void onDiscoveryTimer();

    ThingsRepository& _thingsRepository;

    QTimer _timer;
    QTimer _discoveryTimer;
    uint64_t _currentTimestamp = 0;

    std::list<ThingsDiscoveryPtr> _discoveries;
    std::map<std::string, ThingPtr> _things;
    rpp::subjects::publish_subject<ThingPtr> _thingAdded;

    std::list<Task> _tasks;

    friend class AbstractDiscovery;
};
