#include "Thing.h"

#include <rpp/operators.hpp>

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

template <typename T0, typename ... Ts>
std::ostream& operator<<(std::ostream& s, std::variant<T0, Ts...> const& v){
    std::visit([&](auto && arg) { s << arg; }, v); return s;
}

Thing::Thing(const ThingInfo& info) :
    ThingInfo(info),
    _propertiesObservable(_propertiesSubject.get_observable()/*.multicast(_propertiesSubject)*/) {
    _propertiesObservable.subscribe([this](const auto& val) {
        std::stringstream ss;
        for (const auto& kv : val) {
            ss << "\"" << util::toString(kv.first) << "\": " << kv.second << ", ";
        }
        std::string str = ss.str();
        std::string substr = str.substr(0, str.size()-2);
        LOG_S(INFO) << id() << "> { " << substr << " }";
    });

    //_propertiesSubject.get_subscriber().on_next({{ DynamicProperty::name, cfg->valueOr(id(), Config::Key::name, id()) }});
    // TODO: do not store persistent properties here.
    _persistentProperties[MutableProperty::name] = cfg->valueOr(id(), Config::Key::name, id());
    _persistentProperties[MutableProperty::pinned] = cfg->valueOr(id(), Config::Key::pinned, false);
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

void Thing::setProperty(MutableProperty property, ThingValue value) {
    _persistentProperties[property] = value;
    doSetProperty(property, value);

    // Reflect changes back (to other clients as well).
    const auto property_ = magic_enum::enum_cast<DynamicProperty>(magic_enum::enum_integer(property));
    if (property_.has_value()) {
        LOG_S(INFO) << id() << ".setProperty> " << util::toString(property_.value()) << ": " << value;
        _propertiesSubject.get_subscriber().on_next({{ property_.value(), value }});
    }
}

const std::map<MutableProperty, ThingValue>& Thing::persistentProperties() const {
    return _persistentProperties;
}

dynamic_observable<std::map<DynamicProperty, ThingValue>> Thing::properties() const {
    return _propertiesObservable;
}
