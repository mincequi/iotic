#ifndef ABSTRACTTHING_H
#define ABSTRACTTHING_H

#include <set>
#include <QObject>
#include <iot/ThingInfo.h>

class AbstractThing : public QObject, public ThingInfo {
    Q_OBJECT
public:
    enum class State {
        Connected,
        Failed
    };

    explicit AbstractThing(const ThingInfo& info, QObject *parent = nullptr);

    virtual void read() = 0;

signals:

};

using AbstractThingPtr = std::unique_ptr<AbstractThing>;

#endif // ABSTRACTTHING_H
