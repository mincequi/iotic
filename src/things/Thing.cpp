#include "Thing.h"

#include <rpp/operators.hpp>

#include <common/Logger.h>
#include <config/Config.h>
#include <things/ThingValue.h>
#include <things/sunspec/SunSpecDataPoint.h>

/* TODO: magic_enum stream operator does not work here.
using namespace magic_enum::ostream_operators;
template <typename E>
std::ostream& operator<<(std::ostream& os, const std::pair<E, double>& kv) {
    return os << "{ \"" << util::toString(kv.first) << "\": " << kv.second << " }";
}
*/

Thing::Thing(const ThingInfo& info) :
    ThingInfo(info),
    _propertiesObservable(_propertiesSubject.get_observable()) {
    _propertiesObservable.subscribe([this](const auto& val) {
        std::stringstream ss;
        for (const auto& kv : val) {
            ss << "\"" << util::toString(kv.first) << "\": " << kv.second << ", ";
        }
        std::string str = ss.str();
        std::string substr = str.substr(0, str.size()-2);
        LOG_S(1) << id() << "> { " << substr << " }";
    });

    // Note: no need to emit properties, since there are no subscibers at construction time.
    _mutableProperties[MutableProperty::name] = cfg->valueOr(id(), Config::Key::name, id());
    _mutableProperties[MutableProperty::pinned] = cfg->valueOr(id(), Config::Key::pinned, false);
}

Thing::~Thing() {
}

Thing::Type Thing::type() const {
    return _type;
}

uint16_t Thing::icon() const {
    return _materialIcon;
}

void Thing::read() {
    doRead();
}

void Thing::setProperty(MutableProperty property, const Value& value) {
    // Add property value to local storage for late subscribers
    _mutableProperties[property] = value;

    // Forward value to concrete thing implementation
    doSetProperty(property, value);

    // Reflect changes back (to other clients as well).
    const auto property_ = magic_enum::enum_cast<Property>(magic_enum::enum_integer(property));
    LOG_IF_S(FATAL, !property_.has_value()) << util::toString(property) << " has no readable correspondent";
    LOG_S(1) << id() << ".setProperty> " << util::toString(property_.value()) << ": " << value;
    _propertiesSubject.get_subscriber().on_next({{ property_.value(), value }});
}

const std::map<MutableProperty, Value>& Thing::mutableProperties() const {
    return _mutableProperties;
}

dynamic_observable<std::map<Property, Value>> Thing::properties() const {
    return _propertiesObservable;
}

dynamic_observable<Thing::State> Thing::state() const {
    return _stateSubject.get_observable();
}
