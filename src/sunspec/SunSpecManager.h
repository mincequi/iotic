#ifndef SUNSPECMANAGER_H
#define SUNSPECMANAGER_H

#include <QTimer>

#include "SunSpecModel.h"
#include "SunSpecThing.h"

class SunSpecManager : public QObject {
    Q_OBJECT
public:
    explicit SunSpecManager(QObject *parent = nullptr);

    void startDiscovering(uint16_t seconds = 60);
    void stopDiscovering();

    struct Task {
        std::string thing;
        enum class Op {
            Read
        } type = Op::Read;
        uint16_t modelId = 0;
        uint32_t interval = 0;

        bool operator==(const Task& other);
    };

    void addTask(const Task& task);

signals:
    void thingDiscovered(SunSpecThing*);
    void thingDisappeared(SunSpecThing*);

    void modelRead(SunSpecThing* thing, const SunSpecModel& model);

private:
    void onStartDiscovering();
    void removeThing(SunSpecThing*);

    void onThingStateChanged(SunSpecThing::State state);
    void onTimer();
    void onModelRead(SunSpecThing* thing, uint16_t modelId, uint32_t timestamp, const SunSpecModel& model);

    QList<SunSpecThing*> m_discoveringThings;
    QMap<std::string, SunSpecThing*> m_discoveredThings;

    QList<Task> m_tasks;

    QTimer m_timer;
    uint32_t m_currentTimestamp = 0;

    QTimer m_discoverTimer;
};

#endif // SUNSPECMANAGER_H
