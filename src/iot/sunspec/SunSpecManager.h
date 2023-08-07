#ifndef SUNSPECMANAGER_H
#define SUNSPECMANAGER_H

#include <QTimer>

#include "SunSpecModel.h"
#include "SunSpecThing.h"

namespace sunspec {

class SunSpecManager : public QObject {
    Q_OBJECT
public:
    explicit SunSpecManager(QObject *parent = nullptr);

    void startDiscovery(uint16_t seconds = 60);
    void stopDiscovery();

    struct Task {
        std::string thing;
        enum class Op {
            Read
        } type = Op::Read;
        uint16_t modelId = 0;
        std::chrono::milliseconds intervalMs = std::chrono::milliseconds(0);

        bool operator==(const Task& other);
    };

    void addTask(const Task& task);

signals:
    void thingDiscovered(const sunspec::SunSpecThing&);
    void thingDisappeared(const sunspec::SunSpecThing&);

    void modelRead(const SunSpecThing& thing, const sunspec::Model& model, uint32_t timestamp);

    // TOOD: probably not the right class to emit this. However, this is a simple solution for now.
    void endOfDayReached();

private:
    void onStartDiscovering();
    void removeThing(SunSpecThing*);

    void onThingStateChanged(sunspec::SunSpecThing::State state);
    void onTimer();

    QList<sunspec::SunSpecThing*> m_discoveringThings;
    QMap<std::string, sunspec::SunSpecThing*> m_discoveredThings;

    QList<Task> m_tasks;

    QTimer m_timer;
    uint64_t m_currentTimestamp = 0;

    QTimer m_discoveryTimer;
};

} // namespace sunspec

#endif // SUNSPECMANAGER_H
