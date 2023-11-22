#include "ThingsRepository.h"

#include <common/Logger.h>
#include <things/Site.h>

ThingsRepository* ThingsRepository::instance() {
    if (!_instance) _instance = new ThingsRepository();
    return _instance;
}

ThingsRepository::ThingsRepository() {
}

void ThingsRepository::update() {
    std::erase_if(_things, [this](const auto& t) {
        return _removableThings.contains(t->id());
    });
    _removableThings.clear();
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
    LOG_S(INFO) << "thing added: " << id;
    _things.push_back(std::move(thing));
    _thingAdded.get_subscriber().on_next(_things.back());

    _things.back()->state().subscribe([this, id](auto state) {
        if (state == Thing::State::Failed) {
            LOG_S(WARNING) << "thing completed: " << id;
            // We must not directly delete this thing because thing itself might still process something.
            _thingRemoved.get_subscriber().on_next(id);
            _removableThings.insert(id);
        }
    });
}

const ThingPtr& ThingsRepository::thingById(const std::string& id) const {
    const auto it = std::find_if(_things.begin(), _things.end(), [&](const auto& t) {
        return t->id() == id;
    });
    static const ThingPtr noThing;
    const ThingPtr& ret = (it != _things.end()) ? *it : noThing;
    if (!ret) LOG_S(1) << "thing not found: " << id;
    return ret;
}

const Thing* ThingsRepository::thingByHost(const std::string& host) const {
    auto it = std::find_if(_things.begin(), _things.end(), [&](const auto& t) {
        return t->host() == host;
    });
    return (it != _things.end()) ? it->get() : nullptr;
}

const std::list<ThingPtr>& ThingsRepository::things() const {
    return _things;
}

dynamic_observable<ThingPtr> ThingsRepository::thingAdded() const {
    return _thingAdded.get_observable();
}

dynamic_observable<std::string> ThingsRepository::thingRemoved() const {
    return _thingRemoved.get_observable();
}

void ThingsRepository::setThingProperty(const std::string& id, MutableProperty property, Value value) const {
    const auto& thing = thingById(id);
    if (thing) {
        thing->setProperty(property, value);
    }
}

void ThingsRepository::onRead(const std::string& id, const std::string& response, int error) {
    if (error < 0) {
        LOG_S(WARNING) << "thing completed: " << id;
        // We must not directly delete this thing because thing itself might still process something.
        _thingRemoved.get_subscriber().on_next(id);
        _removableThings.insert(id);
        return;
    }

    const auto& thing = thingById(id);
    if (thing) {
        thing->onRead(response);
    }
}

void ThingsRepository::onReadOnlyError(const std::string& id, int error) {
    if (error < 0) {
        LOG_S(WARNING) << "thing completed: " << id;
        // We must not directly delete this thing because thing itself might still process something.
        _thingRemoved.get_subscriber().on_next(id);
        _removableThings.insert(id);
        return;
    }
}
