#include "AbstractThing.h"

AbstractThing::AbstractThing(const ThingInfo& info, QObject *parent)
    : QObject{parent},
      ThingInfo(info) {
}
