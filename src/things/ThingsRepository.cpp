#include "ThingsRepository.h"

ThingsRepository::ThingsRepository(const std::vector<ThingsDiscovery>& discoveries) :
    _discoveries(discoveries) {


}

void ThingsRepository::addThing(ThingPtr thing) {
    if (auto it = std::find_if(
                _things.begin(),
                _things.end(),
                [&](const auto& t) { return t->id() == thing->id(); } ); it != _things.end()) return;

    _things.push_back(std::move(thing));
    _thingsSubject.get_subscriber().on_next(_things);
}

dynamic_observable<std::vector<ThingPtr>> ThingsRepository::things() const {
    return _thingsSubject.get_observable();
}
