#include "ThingsRepository.h"

ThingsRepository::ThingsRepository() :
    _site(*this) {
}

const Site& ThingsRepository::site() const {
    return _site;
}

void ThingsRepository::addThing(ThingPtr&& thing) {
    auto it = std::find_if(_things.begin(), _things.end(), [&](const auto& t) {
        return t->id() == thing->id();
    });
    if (it != _things.end()) {
        LOG_S(INFO) << "thing already added: " << thing->id();
        return;
    };

    const auto id = thing->id();
    LOG_S(INFO) << "added thing: " << id;
    _things.push_back(std::move(thing));
    _thingsSubject.get_subscriber().on_next(_things);
    _thingAdded.get_subscriber().on_next(_things.back());
}

const ThingPtr& ThingsRepository::thingById(const std::string& id) const {
    const auto it = std::find_if(_things.begin(), _things.end(), [&](const auto& t) {
        return t->id() == id;
    });
    static const ThingPtr noThing = nullptr;
    return (it != _things.end()) ? *it : noThing;
}

const Thing* ThingsRepository::thingByHost(const std::string& host) const {
    auto it = std::find_if(_things.begin(), _things.end(), [&](const auto& t) {
        return t->host() == host;
    });
    return (it != _things.end()) ? it->get() : nullptr;
}

dynamic_observable<std::list<ThingPtr>> ThingsRepository::things() const {
    return _thingsSubject.get_observable();
}

rpp::dynamic_observable<ThingPtr> ThingsRepository::thingAdded() const {
    return _thingAdded.get_observable();
}

void ThingsRepository::setThingProperty(const std::string& id, WriteableThingProperty property, ThingValue value) const {
    const auto& thing = thingById(id);
    if (thing) {
        thing->setProperty(property, value);
    }
}
