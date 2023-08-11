#pragma once

#include <map>
#include <QObject>
#include <QTimer>
#include <things/Thing.h>
#include <things/ThingsDiscovery.h>

class ThingManager : public QObject {
    Q_OBJECT
public:
    struct Task {
        std::string thingId;
        std::chrono::milliseconds intervalMs = std::chrono::milliseconds(0);

        bool operator==(const Task& other);
    };

    explicit ThingManager(QObject *parent = nullptr);

    void startDiscovery();
    void stopDiscovery();

    void addTask(const Task& task);

signals:
    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    void endOfDayReached();

private:
    void addThing(ThingPtr thing);
    void onTimer();

    QTimer _timer;
    uint64_t _currentTimestamp = 0;

    std::list<ThingsDiscoveryPtr> _discoveries;
    std::map<std::string, ThingPtr> _things;
    std::list<Task> _tasks;

    friend class AbstractDiscovery;
};