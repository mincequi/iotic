#include "Thing.h"

#include <rpp/operators.hpp>
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
        LOG_S(INFO) << id() << "> " << "{ " << substr << " }";
    });
}

Thing::~Thing() {
}

Thing::Type Thing::type() const {
    return _type;
}

std::string Thing::name() const {
    return _name;
}

uint16_t Thing::icon() const {
    return _materialIcon;
}

bool Thing::isOnSite() const {
    return _isOnSite;
}

void Thing::read() {
    doRead();
}

dynamic_observable<Thing::State> Thing::state() {
    return _state.get_observable();
}

void Thing::setProperty(WriteableThingProperty property, ThingValue value) {
    if (property == WriteableThingProperty::is_on_site) {
        _isOnSite = std::get<bool>(value);
    }
    doSetProperty(property, value);
}

dynamic_observable<std::map<ReadableThingProperty, ThingValue>> Thing::properties() const {
    return _propertiesObservable;
}
