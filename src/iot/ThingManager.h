#ifndef THINGMANAGER_H
#define THINGMANAGER_H

#include <map>
#include <QObject>
#include <QTimer>
#include <iot/AbstractDiscovery.h>
#include <iot/AbstractThing.h>

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
    void addThing(AbstractThingPtr thing);
    void onTimer();

    QTimer _timer;
    uint64_t _currentTimestamp = 0;

    std::list<AbstractDiscoveryPtr> _discoveries;
    std::map<std::string, AbstractThingPtr> _things;
    std::list<Task> _tasks;

    friend class AbstractDiscovery;
};

#endif // THINGMANAGER_H
