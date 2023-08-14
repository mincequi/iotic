#include "Thing.h"

#include <things/sunspec/SunSpecDataPoint.h>

/* TODO: magic_enum stream operator does not work here.
using namespace magic_enum::ostream_operators;
template <typename E>
std::ostream& operator<<(std::ostream& os, const std::pair<E, double>& kv) {
    return os << "{ \"" << util::toString(kv.first) << "\": " << kv.second << " }";
}
*/

Thing::Thing(const ThingInfo& info) :
    ThingInfo(info) {
    _properties.get_observable()
            .subscribe( [this](const auto& val) {
        LOG_S(INFO) << id() << "> " << "{ \"" << util::toString(val.first) << "\": " << val.second << " }";
    } );
}

Thing::~Thing() {
}

void Thing::read() {
    doRead();
}

dynamic_observable<Thing::State> Thing::state() {
    return _state.get_observable();
}

void Thing::setProperty(WriteableThingProperty property, double value) {
}

dynamic_observable<std::pair<ReadableThingProperty, double>> Thing::properties() {
    /*
    rpp::dynamic_observable<rpp::grouped_observable_group_by<ReadableThingProperty, double>> ret =
            _properties
            .get_observable()
            .group_by([](const auto& pair) { return pair.first; },
                      [](const auto& pair) { return pair.second; });
    return ret;
    */

    return _properties.get_observable();
}
