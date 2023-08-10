#pragma once

#include <memory>
#include <set>
#include <QObject>
#include <things/ThingInfo.h>

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
